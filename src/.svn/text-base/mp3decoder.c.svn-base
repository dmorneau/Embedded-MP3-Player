/**
 * \file mp3decoder.c
 * \author Equipe 01
 * \date 10/08/2010
 *
 * Definition of input cb and ouput cb in
 * order to call madlib decode function
 */
#include "xparameters.h"
#include "mp3decoder.h"
#include "string.h"
#include "madlib/mad.h"
#include "madlib/fixed.h"
#include "errno.h"
#include "scaling.h"
#include "assert.h"
#include "queues.h"
#include "audiobuffer.h"

#define MP3_BUFFER_SIZE 8192
#define NET_MAX_BUFFER 1024

/*	Session information
	Struct to hold session information for a madlib decoding session.
	Currently unused. Could be used to hold a pointer to a message queue, etc.
*/
typedef struct {
	/*	Input buffer. Must be outside the scope of the input callback, since it 
	must stay valid while madlib is decoding. */
	unsigned char mp3buffer[MP3_BUFFER_SIZE];
	/*	ID of the network message queue */
	int input_queue_id;
	/*	Output callback */
	Output_cb user_output;
	/*	Timer, to keep track of the time in the song */
	mad_timer_t songtimer;
} MP3_session;

/*	Madlib input callback

	Called every time Madlib needs more data. Reads a new buffer
	and sends it for processing. Makes the process stop (by returning
	MAD_FLOW_BREAK) if there is an error reading the new buffer.

	\param[in] MP3_session Play session struct
	\param[in] mad_stream Madlib stream
*/
static enum mad_flow input_cb(void* sptr, struct mad_stream *st) {
	unsigned char* readptr;
	MP3_session* session = sptr;
	unsigned char* mp3buffer = session->mp3buffer;
	int bytes_to_read = 0, bytes_read = 0, leftover = 0;

	/*	Prepares the buffer for reading, adjusting it if there was data left 
	over from the previous frame. See:
	http://www.mars.org/pipermail/mad-dev/2003-November/000940.html */
	if (st->next_frame == 0) {
		/*	If there was none we will read a full buffer */
		bytes_to_read = MP3_BUFFER_SIZE;
		readptr = mp3buffer;
	} else {
		/*	Otherwise we will move the remaining data to the beginning of the 
		buffer, and only read enough data to fill the rest of the buffer */
		leftover = st->bufend - st->next_frame;
		memmove(mp3buffer, st->next_frame, leftover);
		readptr = mp3buffer + leftover;
		bytes_to_read = MP3_BUFFER_SIZE - leftover;
	}

#ifdef MP3_VERBOSE
	xil_printf("MP3: reading from queue...\n\r");
#endif

	/*	Read as many chunks from the network as possible to fill the buffer */
	while (bytes_to_read >= NET_MAX_BUFFER) {
		int ret = queue_get(session->input_queue_id, readptr, NET_MAX_BUFFER);
		
		if (ret <= 0)
			break;
			
		bytes_read		+= ret;
		readptr			+= ret;
		bytes_to_read	-= ret;
	}

#ifdef MP3_VERBOSE
	xil_printf("Read %d bytes from file\n", bytes_read);
#endif

	if (bytes_read == 0) {						/* EOF */
		return MAD_FLOW_STOP;
	} else if (bytes_read < 0) {				/* fread error */
		return MAD_FLOW_BREAK;
	} else if (bytes_read < MAD_BUFFER_GUARD) {	/* last frame */
		/* See: http://www.mars.org/pipermail/mad-dev/2001-May/000262.html */
		memset(readptr + bytes_read, 0, MAD_BUFFER_GUARD);
		bytes_read += MAD_BUFFER_GUARD;
	}
	mad_stream_buffer(st, mp3buffer, bytes_read + leftover);
	
	return MAD_FLOW_CONTINUE;
}

/*	Madlib output callback

	Called every time Madlib has PCM data ready.
	Presently, only prints the data to stdout.
	Supports mono and stereo sound (1-2 channels).

	\param[in] MP3_session Play session struct
	\param[in] header Madlib header info
	\param[in] pcm PCM data to output
	\return MAD_FLOW_BREAK if the number of audio channels is not 1 or 2, 
			MAD_FLOW_CONTINUE otherwise.
*/
enum mad_flow output_cb(void* sptr, 
						struct mad_header const *header, struct mad_pcm* pcm) 
{
	int seconds, minutes;
	int nchannels = pcm->channels;
	int nsamples  = pcm->length;
	MP3_session* s = sptr;

	/*	Accumulate the time to show the position in the song */
	mad_timer_add(&(s->songtimer), header->duration);
	//minutes = mad_timer_count(s->songtimer, MAD_UNITS_MINUTES) % 60;
	seconds = mad_timer_count(s->songtimer, MAD_UNITS_SECONDS);
	
	//displays the time on the dio4
	write_dio4_reg2(seconds);
	
	//adjusts the pcm frequency
	audio_set_sampling(XPAR_AUDIO_CODEC_BASEADDR, pcm->samplerate);
	
#ifdef MP3_VERBOSE
	xil_printf("New frame (%d samples, %d channels, %d Hz, %d BPS) - %02d:%02d\n", 
			nsamples, nchannels, pcm->samplerate, header->bitrate, minutes, seconds);
#endif

	/*	Mono output */
	if (nchannels == 1) {
		int *mono = pcm->samples[0];
		while(nsamples--) {
			int sample = scale(*mono);
			sample &= 0x0000FFFF;
			sample |= sample << 16;
			*mono++ = sample;
		}
		/*	Copy back */
		if (s->user_output(pcm->samples[0], pcm->length) == 0)
			return MAD_FLOW_CONTINUE;
		else
			return MAD_FLOW_BREAK;
	}
	
	/*	Stereo output */
	if (nchannels == 2) {
		int *left = pcm->samples[0], *right = pcm->samples[1];
		while(nsamples--) {
			int sample;
			int lsample, rsample;

			lsample = scale(*left);
			rsample = scale(*right);
			sample = (lsample << 16) | rsample;
			
			/*	The left samples side will be used as an output buffer */
			*left = sample;
			left++;
			right++;
		}
		/*	Copy back */
		if (s->user_output(pcm->samples[0], pcm->length) == 0)
			return MAD_FLOW_CONTINUE;
		else
			return MAD_FLOW_BREAK;
	}

	/*	Multichannel (unsupported) output - causes failure */
	return MAD_FLOW_BREAK;
}

/*	Madlib error callback

	Called if madlib hits an internal error. Prints the error code and 
	description to stdout. Attempts to keep decoding.

	\param[in] MP3_session Play session struct
	\param[in] st Madlib stream
	\param[in] fr Madlib frame
	\return MAD_FLOW_CONTINUE if recoverable, MAD_FLOW_BREAK otherwise
*/
enum mad_flow error_cb(void *sptr, struct mad_stream *st, struct mad_frame *fr) {
	int recoverable = MAD_RECOVERABLE(st->error);

	xil_printf(recoverable? "Recoverable" : "Fatal");
	xil_printf(" error 0x%04x (%s) detected. Continuing decoding...\n", 
			st->error, mad_stream_errorstr(st));

	return recoverable? MAD_FLOW_CONTINUE : MAD_FLOW_BREAK;
}

/*	Sets up the madlib callbacks and starts the decoding process.
	Also resets the timer.

	\param[in] input_queue_id ID of the network interface's message queue
	\return 0
*/
int mp3_decode(int input_queue_id, Output_cb out) {
	struct mad_decoder decoder;
	MP3_session session;

	session.songtimer       = mad_timer_zero;
	session.input_queue_id  = input_queue_id;
	session.user_output     = out;

	mad_decoder_init(&decoder, &session, input_cb, 0, 0, output_cb, error_cb, 0);
	mad_decoder_run(&decoder, MAD_DECODER_MODE_SYNC);
	mad_decoder_finish(&decoder);

	return 0;
}
