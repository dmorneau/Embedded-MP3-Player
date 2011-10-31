/**
 * \file main.c
 * \author Equipe 01
 * \date 10/08/2010
 *
 * Main program dowloading files from server
 *
 */
#include "queues.h"
#include "mp3decoder.h"
#include "xmk.h"
#include "semaphore.h"
#include "xparameters.h"
#include "downloader.h"
#include "audiobuffer.h"
#include "interface.h"
#include "xintc.h"
#include "limits.h"
#include "audiobuffer.h"
#include "xac97_l.h"

#define SERVER "132.207.89.02"
#define PORT 9010
char LOCAL_IP[4] = {132,207,89,13};
char SUBNET[4] = {255,255,255,0};
char GATEWAY[4] = {132,207,89,2};

void* socket_thread(void*);
void* task_download(void*);
void* task_decode(void*);
void* task_audio(void*);

ABuffer abuff;

#define DIO4_BASEADDR XPAR_OPB_DIO4_AFFICH_0_BASEADDR
#define AC97_BASEADDR XPAR_AUDIO_CODEC_BASEADDR
#define MAX_VOL 8
#define BUFFER_HALF_FULL 256

Xuint32 vol_g = 4;
short mute_g;// =1 when muted

/*	Volume Control Interface interruption handler */
void dio4_handler(void* base_void) {
	Xuint32 base = (Xuint32)base_void;
	DIO4_state s;
	acknowledge_interrupt(XPAR_OPB_INTC_0_OPB_DIO4_AFFICH_0_IP2INTC_IRPT_INTR);
	s = read_dio4_reg0();
	dio4_clear_interrupt();
	
	if (s.btn1 == 1) {
		if(vol_g > 0)
			vol_g--;
			mute_g = 0;
		print("Diminution du volume.");
		xil_printf(" volume = %d\n\r", vol_g);
	} else if (s.btn2 == 1) {
		if(vol_g < MAX_VOL)
			vol_g++;
			mute_g = 0;
		xil_printf("Augmentation du volume.");
		xil_printf(" volume = %d\n\r", vol_g);
	} else if (s.btn3 == 1) {
			if(mute_g ==0){
				mute_g = 1;
				xil_printf("mute\n\r");
			} else if(mute_g ==1){
				mute_g = 0;	
				xil_printf("unmute");
				xil_printf(" volume = %d\n\r", vol_g);
			}
	}
	if(!mute_g)
	{
		write_dio4_reg1(vol_g);
		audio_set_volume(AC97_BASEADDR, vol_g);
	}
	else 
	{
		write_dio4_reg1(0);
		audio_mute(AC97_BASEADDR);
	}
}


/*	Audio interruption handler */
void* ac97handler(void* unused) {
	int needed;
	acknowledge_interrupt(XPAR_OPB_INTC_0_AUDIO_CODEC_INTERRUPT_INTR);
	needed = BUFFER_HALF_FULL;
	audio_pop(&abuff, XPAR_AUDIO_CODEC_BASEADDR, needed);
}

/*	Download callback. Adds the data to the stream. */
int download_cb(void* arg, char* data, int datasize, fc_status* fc)  {
	return queue_post(NET_QUEUE, data, datasize);
}

int audio_cb(int* data, int nsamples) {
	audio_push(&abuff, data, nsamples);
	return 0;
}

/*	Initializes lwip and starts the application thread */
void* socket_thread(void* arg) {
	int ret;
	DIO4_state val;
	
	lwip_init();
	xil_printf("lwip init completed\n\r");

	register_int_handler(XPAR_OPB_INTC_0_AUDIO_CODEC_INTERRUPT_INTR,
								ac97handler, NULL);
	register_int_handler(XPAR_OPB_INTC_0_OPB_DIO4_AFFICH_0_IP2INTC_IRPT_INTR,
								dio4_handler, (void*)DIO4_BASEADDR);
	
	
	queue_init(NET_QUEUE);
	
	xil_printf("Queue init OK\n\r");
	
	audio_init_buffer(&abuff);
	audio_init_ac97(XPAR_AUDIO_CODEC_BASEADDR);
	
	xil_printf("Audio init OK\n\r");
	audio_enable_interrupt(XPAR_AUDIO_CODEC_BASEADDR);
	dio4_enable_interrupts();
	xil_printf("Intr init OK\n\r");
	display_init();
	xil_printf("Display init OK\n\r");
	
	enable_interrupt(XPAR_OPB_INTC_0_AUDIO_CODEC_INTERRUPT_INTR);
	enable_interrupt(XPAR_OPB_INTC_0_OPB_DIO4_AFFICH_0_IP2INTC_IRPT_INTR);
	
	sys_thread_new(task_download, (void*)NULL, 9);
	sys_thread_new(task_decode  , (void*)NULL, 10);
	xil_printf("Tasks created\n\r");
	return NULL;
}

/*	Attemps to connect the server until it works */
int connect_loop() {
	int socket;
	do { /*	Connect, and keep retrying if the connection fails  */
		xil_printf("Connecting to %s port %d...\n\r", SERVER, PORT);
		socket = fc_connect(SERVER, PORT);
		if (socket == -1)
			xil_printf("Connection failed, retrying...\n\r");
	} while (socket == -1);
	
	return socket;
}

/*	Download the song from server 
   The file #2 on the server is downloaded my default 
*/
void* task_download(void* arg) {
	int socket;
	xil_printf("Download starting...\n\r");
	
	/* Open the socket */
	fc_setup_client(LOCAL_IP, SUBNET, GATEWAY,
						 XPAR_OPB_INTC_0_ETHERNET_MAC_IP2INTC_IRPT_INTR);
	socket = connect_loop();

	xil_printf("Connection success. Downloading...\n\r");
	fc_download_file(socket, "2\0", download_cb, NET_QUEUE);

	/*Clean up*/
	fc_disconnect(socket);
	xil_printf("Download done!\n\r");
	return NULL;
}

/*	Call decoding function */
void* task_decode(void* arg) {
	xil_printf("Decoder task starting.\n\r");
	mp3_decode(NET_QUEUE, audio_cb);
	xil_printf("Decoder task leaving.\n\r");
	return NULL;
}

/*	Main programm */
int main(void) {
	microblaze_init_icache_range(0, XPAR_MICROBLAZE_0_CACHE_BYTE_SIZE);
	microblaze_enable_icache();
	microblaze_init_dcache_range(0, XPAR_MICROBLAZE_0_DCACHE_BYTE_SIZE);
	microblaze_enable_dcache();

	xil_printf("Starting xilkernel.\n\r");
	xilkernel_main();
	return 0;
}
