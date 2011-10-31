/**
 * \file downloader.c
 * \author Equipe 01
 * \date 10/08/2010
 *
 * Connection setup and download fucntions
 *
 */


#include "xmk.h"
#include "xparameters.h"
/*	Necessary to avoid a "redefinition of `struct timeval'" error 
	due to including both XMK and sockets.h */
#define LWIP_TIMEVAL_PRIVATE 0
#include "netif/xemacif.h"
#include "lwip/tcpip.h"
#include "lwip/memp.h"
#include "netif/etharp.h"
#include "lwip/sys.h"
#include "lwip/sockets.h"

#include "downloader.h"
#include "base64decode.h"

/*	Include the XILFS functions only if needed */
#ifdef FC_USE_XILFS
#include "sysace_stdio.h"
#endif /* FC_USE_XILFS */

extern XEmacIf_Config XEmacIf_ConfigTable[];

/*	Buffer size when downloading files */
#define MAXMSG 1024

int fc_setup_client(char local_ip[4], char subnet[4], char gateway[4], int irpt_intr) {
	struct ip_addr ipaddr, netmask, gw;
	struct netif *netif;
	XEmacIf_Config *xemacif_ptr = &XEmacIf_ConfigTable[0];
	
	IP4_ADDR(&gw, gateway[0],gateway[1],gateway[2],gateway[3]);
   IP4_ADDR(&ipaddr,local_ip[0],local_ip[1],local_ip[2],local_ip[3]);
   IP4_ADDR(&netmask,subnet[0],subnet[1],subnet[2],subnet[3]);

	/* Set up network interface */
	netif = mem_malloc(sizeof(struct netif));
	if (netif == NULL) {
		xil_printf("netif_add(): out of memory for default_netif\n\r");
		return -1;
	}
	netif = netif_add(netif, &ipaddr, &netmask, &gw, 
							&XEmacIf_ConfigTable[0], xemacif_init, tcpip_input);
	netif_set_default(netif);
	
	/* Register and enable interrupts */
	register_int_handler(irpt_intr, (XInterruptHandler)XEmac_IntrHandlerFifo, 
								xemacif_ptr->instance_ptr);
	enable_interrupt(irpt_intr);
	
	return 0;
}

int fc_connect(char* server, unsigned short port) {
	int sock, err;
	struct sockaddr_in saddr;
	
	if ( (sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		xil_printf("Error creating socket, ERRNO %d\n\r", errno);
		return -1;
	}
	saddr.sin_family      = AF_INET;
	saddr.sin_port        = htons(port);
	saddr.sin_addr.s_addr = inet_addr(server);

	if( (err = connect(sock,(struct sockaddr *)&saddr, sizeof(saddr))) < 0 )  {
		xil_printf("Error connecting, error %d, ERRNO %d\n\r", err, errno);
		return -1; 
	}

	return sock;
}

int fc_disconnect(int socket) {
	return close(socket);
}

/*	Sends an "OK" to the server */
static int fc_acknowledge(socket) {
	char ok[] = "ok\0";
	write(socket, ok, strlen(ok));
}

/*	Gets an integer value from the server 
	Expects to receive a string from the server.	*/
static int fc_get_int(int socket) {
	char buffer[32]={0};
	read(socket, buffer, 32);
	fc_acknowledge(socket);
	return atoi(buffer);
}

int fc_download_file(int socket, char* filename, fc_callback cb, void* arg) {
	int fileSize, encodedFileSize;
	fc_status status;

	/* Send the selection (file 1, 2 or 3) */
	write(socket,filename,strlen(filename));
	
	/* Get the file size, store it in the info struct */
	fileSize 		 = fc_get_int(socket);
	encodedFileSize = fc_get_int(socket);
	status.totalSize		= fileSize;
	status.remainingSize = fileSize;
	
	/* Get the data chunk-by-chunk in a loop */
	while( encodedFileSize > 0 ) {
		char buffer[MAXMSG];
		char bufferDecoded[MAXMSG];
		int chunkSize, decodedSize;
		
		/* Get the chunk size and the chunk itself */
		chunkSize = fc_get_int(socket);
		if (chunkSize > MAXMSG) {
			xil_printf("Chunksize too large for buffer\n\r");
			break;
		}
		read(socket, buffer, chunkSize);
		
		/* Print parameters (for debugging) */
		/*xil_printf("fileSize %d encodedFileSize %d chunkSize %d\n\r", 
						fileSize, encodedFileSize, chunkSize);*/

		/* Decode the chunk and send it to the callback */
		decodedSize = ap_base64decode(bufferDecoded, buffer, chunkSize);
		/* Taking care to update file size */
		encodedFileSize -= chunkSize;
		status.remainingSize -= decodedSize;
		if (cb(arg, bufferDecoded, decodedSize, &status) == -1) {
			xil_printf("Stopping download\n\r");
			break;
		}
		
		fc_acknowledge(socket);
	}
	
	return 0;
}

#ifdef FC_USE_XILFS
/*	Static variable containing the percents downloaded so far.
	Used to display the progress every X%, instead of every packet.*/
static int fc_percents_downloaded_g;

/*	Callback for the fc_download_file_cf function.
	
	Writes the data back to the CF card, printing a progress report every 5%.
	This function is NOT REENTRANT due to using the global fc_percents_downloaded_g.
	
	fc_percents_downloaded_g must be initialized to 0 before calling this function,
	or else there may be no progress indication. It may also be initialized to -5 to
	display an indication at 0%.
	
	\input arg Output file handle
	\input data Data to write
	\input datasize Size of data to write
	\input fc File client information struct
	\output 0 if the write was successful, -1 otherwise.
*/
static int fc_download_file_cf_cb(void* arg, char* data, int datasize, fc_status* fc) {
	int completed, pc;
	int oldpc = fc_percents_downloaded_g;
	
	/*	Calculate the percentage done and remaining file size */
	completed = fc->totalSize - fc->remainingSize;
	pc = (completed * 100) / fc->totalSize;
	
	/*	Print a pretty message every 5% */
	if (pc - oldpc >= 5) {
		xil_printf("%2d%% - %d bytes remaining\n\r", pc, fc->remainingSize);
		fc_percents_downloaded_g = pc;
	}
	
	/*	And write the data back to the compact flash card*/
	return sysace_fwrite(data, 1, datasize, arg) > 0 ? 0 : -1;
}

int fc_download_file_cf(int socket, char* name_server, char* name_local) {
	void *file = sysace_fopen(name_local, "w");
	if (file == 0) {
		xil_printf("Could not open file\n\r");
		return -1;
	} else {
		fc_percents_downloaded_g = -5;
		fc_download_file(socket, name_server, fc_download_file_cf_cb, file);
		sysace_fclose(file);
		return 0;
	}
}
#endif
