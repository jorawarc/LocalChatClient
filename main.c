/* main.c
 *
 * Description:
 * Chat client, using server/client model 
 * Uses UDP sockets to communicate over a network
 *
 * Author: Jorawar Chana
 * Data Last Modified: Feb. 10, 2018
 *
 */

#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdio.h>
#include <netdb.h>
#include <string.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <pthread.h>
#include "s-talk.h"
#include "List.h"
#include "threading.h"


int main(int argc, char** argv){

// -- Threading Variables -- //

	pthread_t* threads = getThreads();
	struct senderAndReciever_thread senderAndReciever_data[NUM_THREADS_NET];
	struct monitorIO_thread inputOutput_data[NUM_THREADS_IO];

	int sender_thread, reciever_thread, keyboard_thread, monitor_thread;

	// Thread Ids
	int sender_id = getSenderID();
	int reciever_id = getRecieverID();
	int keyboard_id = getKeyboardID();
	int monitor_id = getMonitorID();

	// Thread data locations
	int sender_data = 0;
	int keyboard_data = 0;
	int reciever_data = 1;
	int monitor_data = 1;

// -- Data collection -- //
	LIST* messagesToSend = ListCreate();
	LIST* messagesToOutput = ListCreate();

// -- Network Utility Variables -- //
	char buf[SIZE];
	char msg[SIZE];
	int n, length;

	int LOCAL_PORT = atoi(argv[1]);
	int REMOTE_PORT = atoi(argv[3]);
	char REMOTE_MACHINE_NAME[128];
	strcpy(REMOTE_MACHINE_NAME, argv[2]);

// -- Network Functional Variables -- //
	int NETWORK_SOCKET_HOST, NETWORK_SOCKET_CLIENT;
	struct sockaddr_in HOST, CLIENT;
	struct hostent *Client_Info;

// -- Network Logic Code -- //

	NETWORK_SOCKET_HOST = socketCreate();
	NETWORK_SOCKET_CLIENT = socketCreate();

	hostCreate(&HOST,LOCAL_PORT);
	clientCreate(&CLIENT, REMOTE_MACHINE_NAME, REMOTE_PORT, &Client_Info);
	bindInitialization(NETWORK_SOCKET_HOST, &HOST, sizeof(HOST));

	bzero(msg,SIZE);
	bzero(buf,SIZE);

// -- Setup Sender thread -- //
	senderAndReciever_data[sender_data].socket = NETWORK_SOCKET_CLIENT;
	senderAndReciever_data[sender_data].msg = msg;
	senderAndReciever_data[sender_data].msgLen = strlen(msg);
	senderAndReciever_data[sender_data].clientAddr = &CLIENT;
	senderAndReciever_data[sender_data].size = sizeof(CLIENT);
	senderAndReciever_data[sender_data].sharedData = messagesToSend;


// -- Setup Reciever thread -- //
	senderAndReciever_data[reciever_data].socket = NETWORK_SOCKET_HOST;
	senderAndReciever_data[reciever_data].msg = buf;
	senderAndReciever_data[reciever_data].msgLen = SIZE;
	senderAndReciever_data[reciever_data].clientAddr = &HOST;
	senderAndReciever_data[reciever_data].size = sizeof(HOST);
	senderAndReciever_data[reciever_data].sharedData = messagesToOutput;

// -- Setup Keyborad thread -- //
	inputOutput_data[keyboard_data].list = messagesToSend; 
	inputOutput_data[monitor_data].list = messagesToOutput; 

// -- Start all threads -- //
	sender_thread = pthread_create(&threads[sender_id], NULL, send_thread, &senderAndReciever_data[sender_data]);
	reciever_thread = pthread_create(&threads[reciever_id], NULL, recv_thread, &senderAndReciever_data[reciever_data]);
	monitor_thread  = pthread_create(&threads[monitor_id],NULL, monitorOutput_thread, &inputOutput_data[monitor_data]);
	keyboard_thread = pthread_create(&threads[keyboard_id], NULL, keyboardInput_thread, &inputOutput_data[keyboard_data]);



// -- Wait until threads are done executing -- // 
	int isSessionOver = 0;
	while(!isSessionOver){
		if(pthread_join(threads[keyboard_id], NULL) == 0){
			isSessionOver = 1;
		}

		else if(pthread_join(threads[reciever_id],NULL) == 0){
			isSessionOver = 1;
		}

	}


	printf("Exiting...\n");
	printf("Good Bye!\n");
	exit(0);

}