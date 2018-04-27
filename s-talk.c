/* s-talk.c
 *
 * Description:
 * Implements various newtwork connection requirements
 * Program will exit if error on any critial sections
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


// Utility method to output error message, and exit program
void printError(const char msg[]){
	fprintf(stderr,"%s\n",msg);
	exit(1);
	return;
}

// Debugging method, used to print out info
void printLocalInfo(int LOCAL_PORT, char REMOTE_MACHINE_NAME[], int REMOTE_PORT){
	printf("LOCAL_PORT: %d\nREMOTE_MACHINE_NAME: %s\nREMOTE_PORT: %d\n",LOCAL_PORT,REMOTE_MACHINE_NAME,REMOTE_PORT);
	return;
}

// Creates a UDP socket
// returns socket file descriptor
int socketCreate(){
	int fileDescriptor = socket(AF_INET, SOCK_DGRAM,0);
	if(fileDescriptor == -1){
		printError("socket() in socketCreate() returned error: quitting");
	}
	return fileDescriptor;
}

// Initializes Host information
void hostCreate(struct sockaddr_in* hostAddr, int localPort){
	if(!hostAddr){
		printError("Host Address is NULL: quitting");
	}

	hostAddr->sin_family = AF_INET;
	hostAddr->sin_port = htons(localPort);
	hostAddr->sin_addr.s_addr = INADDR_ANY;
	if(!memset(&hostAddr->sin_zero, '\0',8)){
		printError("memset() in hostCreate() is NULL: quitting");
	}
	return;
}


// Initialize client info
void clientCreate(struct sockaddr_in* clientAddr, char hostName[], int portNum, struct hostent** Client_Info){
	clientAddr->sin_family = AF_INET;
	*Client_Info = gethostbyname(hostName);
	if(!(*Client_Info)){
		printError("gethostbyname() in clientCreate() is NULL: quitting");
	}
	clientAddr->sin_port = htons(portNum);
	bcopy((char *)(*Client_Info)->h_addr, (char *)&clientAddr->sin_addr,(*Client_Info)->h_length);
	return;
}


// Bind host to socket
void bindInitialization(int socket, struct sockaddr_in* hostAddr, int len){
	int bindCheck = bind(socket,(struct sockaddr*) hostAddr, len);
	if(bindCheck == -1){
		printError("bind() in bindInitialization() returned error: quitting");
	}
	return;
}

// Send messages to client
int sendMsg(int socket, char* msg, int msgLen, struct sockaddr_in* clientAddr, int size){
	int bytesSent = sendto(socket, msg,msgLen,0,(struct sockaddr *) clientAddr, size);
	if(bytesSent == -1){
		printError("sendto() in sendMsg() returned error: quitting");
	}
	return bytesSent;
}


// Receive messages from client
int receiveMsg(int socket, char* buffer, int bufferLen, struct sockaddr_in* clientAddr, int* size){
	int bytesRecieved = recvfrom(socket,buffer,bufferLen,0,(struct sockaddr *) clientAddr,size);
	if(bytesRecieved == -1){
		printError("recvfrom() in receiveMsg() returned error: quitting");
	}
}