/**
 * \file queue.c
 * \author Equipe 01
 * \date 10/08/2010
 *
 * Implementation of a circular queue
 * 
 */

#include "queues.h"
#include "sys/msg.h"
#include "sys/ipc.h"
#include "errno.h"

/*	Queue initialization 
   Creates queue 
	\param[in] int id
*/
int queue_init(int id) {
	int qid = msgget(id, IPC_CREAT);
	if (qid == -1) {
		xil_printf("Could not create queue #%d, errno is %d.\n\r", id, errno);
		return -1;
	}
	return qid;
}


/*	Queue Post
   Sends data 
	\param[in] int id
	\param[in] char* buffer
	\param[in] int size
*/
int queue_post(int id, char* buffer, int size) {
	int ret;
	
	ret = msgsnd(id, &size, sizeof(size), 0);
	if (ret != 0) {
		xil_printf("Queue post error (sending size): %d.\n\r", errno);
		return ret;
	}
	
	ret = msgsnd(id, buffer, size, 0);
	if (ret != 0)
		xil_printf("Queue post error: %d (size was %d).\n\r", errno, size);
	return ret;
}

/*	Queue Get
   Obtain data from queue
	\param[in] int id
	\param[in] char* buffer
	\param[in] int size
*/
int queue_get(int id, char* buffer, int size) {
	int ret, msgsize;
	
	ret = msgrcv(id, &msgsize, sizeof(msgsize), 0, 0);
	if (ret <= 0) {
		xil_printf("Queue get error: could not get msgsize from queue %d, errno %d\n\r",
					  id, errno);
		return ret;
	}
	if (msgsize > size) {
		xil_printf("Queue get error: buffer too small to get message "
					  "(%d, but %d is required), errno %d\n\r", size, msgsize, errno);	
		return -1;
	}
	
	ret = msgrcv(id, buffer, msgsize, 0, 0);
	if (ret <= 0)
		xil_printf("Queue get error: could not get msg from queue %d, errno %d\n\r", id, errno);
	return ret;
}
