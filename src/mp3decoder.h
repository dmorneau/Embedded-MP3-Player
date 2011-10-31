/*	\file MP3 decoding module - PC version
	
	This main.c file is a PC implementation of the MP3 decoding module of the 
	project. Main differences:
	- Input is simulated by reading a file, instead of getting data from the 
	Ethernet module
	- No output is done, except printing an information message with the time 
	in the song to the standard output
	- Printf is used
	- No quantization is done. 32-bits samples are considered final.

	#define MP3_VERBOSE to get printf output

	Work to do to adapt this to the pipeline:
	Perhaps no need for extra buffering on the audio side - there is already 
	a fairly deep buffer on the AC97 OPB adapter.

	For the network side, we need a way to go from fixed-size packets to 
	flexibly sized amounts of data, since the library eats frame-sized 
	pieces of data only.


	See document ML505_MB_XMK_92.pdf for info on message queues -
	must include "sys/msg.h" and "sys/ipc.h" to work.


	To implement dithering:
	Use a PRNG with triangular distribution
	-> use a generic uniform PRNG and address into a LUT to generate the 
	   distribution
   PRNG source: http://groups.google.com/group/comp.lang.c/browse_thread/thread/a9915080a4424068/
    Shift the PRN to be just under the target bit depth and add to the sample
	Round, clip, quantize the sample

	To implement noise shaping:
	Use a FIR. More concretely, just keep a value in memory to represent the error
	and try to maintain error close to 0?

	Notes personnelles pour la redaction du rapport -- Dominic

	Using the timer API to track time:
	http://www.mars.org/pipermail/mad-dev/2001-May/000244.html

	Exemples:
	http://code.google.com/p/libgdx/source/browse/trunk/jni/NativeMP3Decoder.cpp?r=9
	http://svn.sbooth.org/play/tags/release-0.1.2/Audio/Decoders/MPEGStreamDecoder.m

	Notes for porting to Microblaze

	Define FPM_DEFAULT in compiler options to compile.

	Suggestion: add MADD support to the CPU and #define HAVE_MADD_ASM, FPM_MIPS ?

	quantization (scale) = TODO

	References:
	http://m.baert.free.fr/contrib/docs/libmad/doxy/html/high-level.html
	http://www.mars.org/pipermail/mad-dev/2001-October/000369.html
	http://www.mars.org/pipermail/mad-dev/2003-November/000940.html  stream_buffer alignment
	http://www.mars.org/pipermail/mad-dev/2001-May/000262.html last frame MAD_BUFFER_GUARD
*/
#ifndef MP3_DECODER_H
#define MP3_DECODER_H
#include <xbasic_types.h>

typedef int (*Output_cb)(int* buff, int nsamples);

/*	Sets up the madlib callbacks and starts the decoding process.
	Also resets the timer.
	\return 0
*/
int mp3_decode(int input_queue_id, Output_cb out);

#endif
