/**
 * \file audiobuffer.c
 * \author Equipe 01
 * \date 10/08/2010
 *
 * Definition of input cb and ouput cb in
 * order to call madlib decode function
 */
#include "audiobuffer.h"
#include "xmk.h"
#include "xstatus.h"
#include "xac97_l.h"
#include "xparameters.h"

/*	Inspired from 
http://stackoverflow.com/questions/827691/
how-do-you-implement-a-circular-buffer-in-c */

/*	AudioBuffer intialization

	\param[in] ABuffer *ab	
*/
void audio_init_buffer(ABuffer *ab) {
	ab->ri = 0;
	ab->wi = 0;
	if (sem_init(&ab->sem, 0, 0) != 0)
		xil_printf("AUDIO: Error creating the semaphore\n\r");
}

/*	Audio Codec Intialization

	\param[in] Xuint32 ac97
*/
void audio_init_ac97(Xuint32 ac97) {
	XAC97_HardReset(ac97);
	XAC97_WriteReg(ac97, AC97_MasterVol, AC97_VOL_ATTN_9_0_DB);
	XAC97_WriteReg(ac97, AC97_AuxOutVol, AC97_VOL_ATTN_9_0_DB);
	XAC97_WriteReg(ac97, AC97_PCMOutVol, AC97_VOL_ATTN_9_0_DB);
	XAC97_WriteReg(ac97, AC97_ExtendedAudioStat, 
						AC97_EXTENDED_AUDIO_CONTROL_VRA);
	XAC97_WriteReg(ac97, AC97_PCM_DAC_Rate, AC97_PCM_RATE_48000_HZ);
}

/*	Audio Interrup Activation

	\param[in] Xuint32 ac97
*/
void audio_enable_interrupt(Xuint32 ac97) {
	int i = 16;
	XAC97_SoftReset(ac97);
	while (i--)
		XAC97_WriteFifo(ac97, 0x00FF00FF);
	XAC97_mSetControl(ac97, AC97_ENABLE_OUT_FIFO_INTERRUPT);
}

/*	Helper function, calculates the space left in the buffer */
static int audio_get_writable(ABuffer *ab) {
	int remaining = 0;

	/*      RI > WI
	  ------------------- 
	     ^           ^
		 WI          RI 
	Remaining is ((ri - 1) - wi)
	         WI > RI 
	  ------------------- 
	     ^           ^
		 RI          WI 
	Remaining is (WI to end) plus (0 to RI)
			WI == RI
		Assume an empty buffer
	*/
	if (ab->ri == ab->wi) {
		remaining = AUDIO_BUFFER_SIZE - 1;
	} else if (ab->ri > ab->wi) {
		remaining = ab->ri - ab->wi - 1;
	} else {
		int write_to_end = (AUDIO_BUFFER_SIZE - 1) - ab->wi;
		remaining = write_to_end + ab->ri;
	}
	
	return remaining;
}

/*	Helper function, calculates the space left in the buffer */
static inline int audio_get_readable(ABuffer* ab) {
	return AUDIO_BUFFER_SIZE - audio_get_writable(ab);
}


/*	Audio Push Function

	\param[in] ABuffer *ab
	\param[in] int *data
	\param[in] int nsamples
*/
void audio_push(ABuffer *ab, int *data, int nsamples) {
	int i, idx;

	while (audio_get_writable(ab) < nsamples)
		sem_wait(&ab->sem);

	for (i = 0; i < nsamples; ++i) {
		idx = (ab->wi + i) & (AUDIO_BUFFER_SIZE - 1);
		ab->data[idx] = data[i];
	}

	ab->wi += nsamples;
	ab->wi &= (AUDIO_BUFFER_SIZE - 1);
}

/*	Audio Pop Function

	\param[in] ABuffer *ab
	\param[in] Xuint32 ac97
	\param[in] int nsamples
*/
void audio_pop(ABuffer *ab, Xuint32 ac97, int nsamples) {
	int i, idx;

	if (audio_get_readable(ab) < nsamples) {
		while (nsamples--)
			XAC97_WriteFifo(ac97, 0x00FF00FF);
		goto end;
	}

	for (i = 0; i < nsamples; ++i) {
		idx = (ab->ri + i) & (AUDIO_BUFFER_SIZE - 1);
		XAC97_WriteFifo(ac97, ab->data[idx]);
	}

	ab->ri += nsamples;
	ab->ri &= (AUDIO_BUFFER_SIZE - 1);

end:
	sem_post(&ab->sem);
	return;
}

/* PCM Frenquency Adjustment
   
	\param[in] Xuint32 freq
*/

void audio_set_sampling(Xuint32 address, Xuint32 freq) {
	XAC97_WriteReg(address, AC97_PCM_DAC_Rate, freq);
}
/*	Ac97 Volume 
   Volume has been divided into 8 levels to
   correspond to 8 leds	
	\param[in] Xuint32 volume
*/
void audio_set_volume(Xuint32 address, int volume) {
	Xuint32 v = 0;
	
	#define VOL(val, def) case val: v = def; break; 
	switch(volume)
	{
		VOL(0, AC97_VOL_ATTN_44_5_DB);
		VOL(1, AC97_VOL_ATTN_41_5_DB);
		VOL(2, AC97_VOL_ATTN_35_5_DB);
		VOL(3, AC97_VOL_ATTN_29_5_DB);
		VOL(4, AC97_VOL_ATTN_23_5_DB);
		VOL(5, AC97_VOL_ATTN_17_5_DB);
		VOL(6, AC97_VOL_ATTN_11_5_DB);
		VOL(7, AC97_VOL_ATTN_6_0_DB);
		VOL(8, AC97_VOL_ATTN_0_DB);
		default: break;
	}
	XAC97_WriteReg(address, AC97_PCMOutVol, v);
	XAC97_WriteReg(address, AC97_MasterVol, AC97_VOL_MAX );
	XAC97_WriteReg(address, AC97_AuxOutVol, AC97_VOL_MAX);
	XAC97_WriteReg(address, AC97_MasterVolMono, AC97_VOL_MAX);	
}

void audio_mute(Xuint32 address) {
	XAC97_WriteReg(address, AC97_MasterVol, AC97_VOL_MUTE );
	XAC97_WriteReg(address, AC97_MasterVol, AC97_VOL_MUTE );
	XAC97_WriteReg(address, AC97_AuxOutVol, AC97_VOL_MUTE);
	XAC97_WriteReg(address, AC97_MasterVolMono, AC97_VOL_MUTE);	
}
