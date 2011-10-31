#ifndef AUDIOBUFFER_H
#define AUDIOBUFFER_H
#include "xbasic_types.h"
#include "semaphore.h"

/*	Audio buffer size, MUST BE A POWER OF TWO */
#define AUDIO_BUFFER_SIZE 65536

/*	Audio buffer
	Holds the values to be output on the AC97 device.
	Supports a very limited interface based on push/pop.
*/
typedef struct {
	unsigned int wi;
	unsigned int ri;
	int data[AUDIO_BUFFER_SIZE];
	sem_t sem;
} ABuffer;

/*	Initialize an audio buffer
	Sets the write and read index of the buffer to 0.
	\param ab Audio buffer struct to initialize
*/
void audio_init_buffer(ABuffer *ab);

/*	Initializes the AC97 
	Performs a hard reset and sets up some default values.
	\param ac97 AC97 base address
*/
void audio_init_ac97(Xuint32 ac97);

/*	Enables AC97 interrupt

*/
void audio_enable_interrupt(Xuint32 ac97);

/*	Stores a number of audio samples
	If the buffer is full, this will block on a semaphore until
	enough samples are read using audio_pop.
	\param ab Audio buffer to write into
	\param data Data to copy
	\param nsamples Size of the data array
*/
void audio_push(ABuffer *ab, int *data, int nsamples);

/*	Sends a number of samples to the AC97 
	If the buffer is empty, this will send dummy data (0x00FF00FF)
	to keep the AC97 FIFO happy (of course, the music will be cut
	until more samples are pushed into the buffer).
	\param ab Audio buffer to read from
	\param ac97 AC97 base address
	\param nsamples Number of samples to send
*/
void audio_pop(ABuffer *ab, Xuint32 ac97, int nsamples);

/*	
	Sets the sampling rate of the ac97
*/

void audio_set_sampling(Xuint32 address, Xuint32 freq);

/*	Sets the AC97 volume
	Takes a volume between 0 and 8.
*/
void audio_set_volume(Xuint32 address, int volume);

/*	Mutes the AC97 output (via AC97_MasterVol)
*/
void audio_mute(Xuint32 address);

#endif
