#ifndef DOWNLOADER_H__
#define DOWNLOADER_H__
/**\file downloader.h
   \author Equipe 01
   \date 10/08/2010

	File download client
	
	This file implements functions to get a file from the INF3990 file server.
	If using the SystemACE Compact Flash driver, defining FC_USE_XILFS in the 
	compiler options will include the SystemACE header files and implement a 
	simple function (fc_download_file_cf()) to download files onto the CF card.
	
	Otherwise, fc_download_file can be used with a custom callback to process the 
	data as it transfers.
	
	Proper usage is:
	1- Configure the network interface and register interruptions with fc_setup_client
	2- Connect a new socket to the server with fc_connect
	3- Download files with fc_download_file_cf or fc_download_file and a callback
	4- Close the socket with fc_disconnect
*/

/*	Struct to hold information on a download.
	The download callback can use this to calculate progress,
	etc. */
typedef struct {
	int totalSize;
	int remainingSize;
} fc_status;

/*	Sets up the file download client.

	Initializes the network interface, then registers and enables 
	interruptions. Returns -1 on error, 0 on success.
	
	Usage example:
	char LOCAL_IP[4] = {132,207,89,22};
	char SUBNET[4] = {255,255,255,0};
	char GATEWAY[4] = {132,207,89,2};
	
	err = fc_setup_client(
			LOCAL_IP,
			SUBNET,
			GATEWAY,
			XPAR_OPB_INTC_0_ETHERNET_MAC_IP2INTC_IRPT_INTR );
			
	\input local_ip IP to assign to the network interface
	\input subnet IP mask
	\input gateway Router IP
	\input irp_intr Value defined in xparameters.h for the Ethernet device
				interrupts.
*/
int fc_setup_client(char local_ip[4], char subnet[4], char gateway[4], 
						  int irpt_intr);

/*	Connects to a file server

	Takes a string with the server name in dot-delimited format,
	a port, and returns a (TCP) socket connected to the server. If 
	an error happens, returns -1.
	
	Call fc_setup_client before this function.
	
	Usage example:
	socket = fc_connect("132.207.89.02", 9010);
	
	\input server Server IP to connect to, as a dot-delimited string.
	\input port Port number (0-65535)
*/
int fc_connect(char* server, unsigned short port);

/*	Disconnects from a file server

	Actually just a wrapper for close(socket).
	
	\input socket Socket to close
	\output Return value of close()
*/
int fc_disconnect(int socket);

/*	Type of a callback for fc_download_file.
	Corresponds to the following signature:
	int my_cb(void* arg, char* data, int datasize, fc_status* status)
*/
typedef int(*fc_callback)(void*, char*, int, fc_status*);

/* Downloads a file, invoking a callback whenever data arrives

	Takes a socket (created with fc_connect) and a filename.
	Downloads the file and regularly calls the callback as new data 
	arrives.
	
	The callback will be invoked with the void* pointer given as 
	extra argument, and a pointer to the data buffer. This buffer may 
	be edited in-place if needed; fc_download_file does not reuse its 
	content after sending it to the callback.
	
	The callback may return -1 to stop the download. Any other value 
	means the download shall keep going.
	
	The callback should have an fc_callback signature:
	int my_cb(void* arg, char* data, int datasize, fc_status* status)
	
	This function does not save anything to the CF card nor does it print 
	progress information; this is the callback's responsibility.
	
	\input socket Socket to use. Must have been initialized with 
		fc_setup_client and fc_connect first.
	\input filename Filename to send to the server. Should be a string like 
		"1\0", "2\0" or "3\0".
	\input fc_callback New data callback. Will be called everytime a chunk is 
		received. Chunks are limited to MAXMSG (defined in downloader.c) in size.
		By defaults, chunks are under 1k in size.
	\input arg Callback arguments. Will be provided to the callback on every call.
		This can be any value needed for the user logic.
*/
int fc_download_file(int socket, char* filename, fc_callback cb, void* arg);


/*	Connect to the file server, and keep retrying if the connection fails  */
int connect_loop();



#ifdef FC_USE_XILFS
/*	Downloads a file, saving it on the compact flash card 
	
	This fonction is implemented only if FC_USE_XILFS is defined (add
	-D FC_USE_XILFS to the compiler options).

	\input socket Socket to write on
	\input name_server Filename on the server (use "1\0", "2\0" or "3\0")
	\input name_local Filename to use on the Compact Flash card. Must 
					respect FAT16 convention (6 characters, dot, 3 characters).
*/
int fc_download_file_cf(int socket, char* name_server, char* name_local);
#endif /* FC_USE_XILFS */

#endif
