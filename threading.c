/* threading.c
 *
 * Description:
 * Implements threading functions for chat client
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

enum threadIndexID{
	sender = 0,
	reciever = 1,
	keyboard = 2,
	monitor = 3
};


pthread_t threads[NUM_THREADS_NET + NUM_THREADS_IO];


// Returns pointer to threads
pthread_t* getThreads(){
	return threads;
}

// Returns Sender thread ID
int getSenderID(){
	return sender;
}

// Returns Reciever thread ID
int getRecieverID(){
	return reciever;
}

// Returns Keyboard thread ID
int getKeyboardID(){
	return keyboard;
}

// Returns Monitor thread ID
int getMonitorID(){
	return monitor;
}



// Manage sender thread
// Data should be a pointer to a senderAndReciever object 
void* send_thread(void* data){


	// Access data structure
	struct senderAndReciever_thread* localData = (struct senderAndReciever_thread *) data;
	int socket = localData->socket;
	struct sockaddr_in* clientAddr = localData->clientAddr;
	int sizeClient = localData->size;
	LIST* messagesToSendList = localData->sharedData;


	int isEscapeChar = 0;

	while(!isEscapeChar){
		pthread_mutex_lock(&sent_mutex);

		// If there are no messages to be sent then wait
		if(ListCount(messagesToSendList) == 0){
			pthread_cond_wait(&messageReadyToSend_cond, &sent_mutex);
		}

		// Find most recent message in list
		ListLast(messagesToSendList);

		// Allocate memory, and clear it
		int msgLen = strlen(ListCurr(messagesToSendList));
		char msg[msgLen];
		bzero(msg,msgLen);

		// Make local copy of message
		strcpy(msg, ListCurr(messagesToSendList));

		// Check if message is the terminating character
		if(strcmp(msg,ESCAPE_CHAR) == 0){
			isEscapeChar = 1;
		}

		// Message and clear from memory
		sendMsg(socket,msg,msgLen, clientAddr, sizeClient);
		ListTrim(messagesToSendList);
		bzero(msg,msgLen);

		pthread_cond_signal(&messageReadyToSend_cond);
		pthread_mutex_unlock(&sent_mutex);

	}
	printf("Closing socket connection...\n");
	close(socket);

	pthread_cond_destroy(&messageReadyToSend_cond);
	pthread_mutex_destroy(&sent_mutex);
}

// Manage reiever thread
// Data should be a pointer to a senderAndReciever object 
void* recv_thread(void* data){

	// Accesss data structure
	struct senderAndReciever_thread* localData = (struct senderAndReciever_thread *) data;
	int socket = localData->socket;
	struct sockaddr_in* clientAddr = localData->clientAddr;
	int length = localData->size;
	LIST* messagesReceivedList = localData->sharedData;
	int bufSize = localData->msgLen;

	// --

	char* buf = (char*) malloc(sizeof(char)*SIZE);
	bzero(buf,SIZE);

	// -- END
	//char* buf = localData->msg;


	int bytesRecieved = 1;
	int isEscapeChar = 0;


	while(!isEscapeChar){
		bytesRecieved = receiveMsg(socket, buf, bufSize, clientAddr, &length);

		if(strcmp(buf,ESCAPE_CHAR) != 0 && bytesRecieved > 0){
			pthread_mutex_lock(&message_mutex);

			// If there are messages that still need to be outputed, then wait
			if(ListCount(messagesReceivedList) > 0){
				pthread_cond_wait(&messageRecieved_cond,&message_mutex);
			}

			// Add recieved messaged to list
			ListAppend(messagesReceivedList, buf);

			pthread_cond_signal(&messageRecieved_cond);
			pthread_mutex_unlock(&message_mutex);
			
		}

		else{
			isEscapeChar = 1;
		}
	}

	printf("Recieved trerminating character.\n");
	printf("Closing threads...\n");
	sleep(3);


	printf("Closing socket connection...\n");
	close(socket);

	printf("Freeing Memory...\n");
	free(buf);

	printf("Cleaning up...\n");
	pthread_cond_destroy(&messageRecieved_cond);

	pthread_cancel(threads[monitor]);
	pthread_cancel(threads[keyboard]);
	pthread_cancel(threads[sender]);

	pthread_exit(NULL);
}


// Manage keyboard monitor thread
// Recieved data should be stored in a global List object
// Critical Section for List object in send thread and keyboard monitor thread
void* keyboardInput_thread(void* data){
	struct monitorIO_thread* localData = (struct monitorIO_thread *) data;
	LIST* messageToBeSentList = localData->list;


//--
	char* buffer = (char*) malloc(sizeof(char)*SIZE);
	bzero(buffer,SIZE);

	if(buffer == NULL){
		printf("buffer == NULL\n");
	}

	int isEscapeChar = 0;

	// Escape character to exit program is: '!'
	while(!isEscapeChar){
		pthread_mutex_lock(&sent_mutex);

		// If there are messages that still need to be sent, wait
		if(ListCount(messageToBeSentList) >= 1){
			pthread_cond_wait(&messageReadyToSend_cond,&sent_mutex);
		}

		// Clear buffer and add new keystrokes to list
		bzero(buffer,SIZE);
		read(STDIN_FILENO, buffer, SIZE);
		ListPrepend(messageToBeSentList, buffer);

		if(strcmp(buffer,ESCAPE_CHAR) == 0){
			isEscapeChar = 1;
		}

		pthread_cond_signal(&messageReadyToSend_cond);
		pthread_mutex_unlock(&sent_mutex);
	}

	// Wait for other threads to finish
	printf("Terminal character transmitted.");
	printf("Closing threads..\n");
	sleep(3);

	printf("Freeing Memory..\n");
	free(buffer);

	printf("Cleaning up...\n");
	pthread_cancel(threads[sender]);
	pthread_cancel(threads[reciever]);
	pthread_cancel(threads[monitor]);



	pthread_exit(NULL);
	
}


// Manges screen thread
// Outputs Recieved messages to the users screen
// Critical Section for List object between screen, and recive thread
void* monitorOutput_thread(void* data){

	int isEscapeChar = 0;

	while(!isEscapeChar){
		struct monitorIO_thread* localData = (struct monitorIO_thread *) data;
		LIST* messagesToOutput = localData->list; 

		if(messagesToOutput != NULL && ListCount(messagesToOutput) != 0){
			
			pthread_mutex_lock(&message_mutex);

			// If there are no messages to be outputed then wait
			if(ListCount(messagesToOutput) == 0){
				pthread_cond_wait(&messageReadyToOutput_cond,&message_mutex);
			}

			// Locate correct message, and output to user
			ListLast(messagesToOutput);
			printf("%s",(char*) ListCurr(messagesToOutput));

			if(strcmp(ListCurr(messagesToOutput),ESCAPE_CHAR) == 0){
				isEscapeChar = 1;
			}

			bzero(ListTrim(messagesToOutput),SIZE);

			pthread_cond_signal(&messageReadyToOutput_cond);
			pthread_mutex_unlock(&message_mutex);
		}
		
	}

	pthread_cond_destroy(&messageReadyToOutput_cond);
	pthread_mutex_destroy(&message_mutex);

}
