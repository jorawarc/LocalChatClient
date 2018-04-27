/* s-talk.h
 *
 * Description:
 * Abstracts network connections away
 *
 * Author: Jorawar Chana
 * Data Last Modified: Feb. 10, 2018
 *
 */

// Max buffer size for sending and recieving messages
#define SIZE 1024

// Debugging method, used to print out info
void printLocalInfo(int LOCAL_PORT, char REMOTE_MACHINE_NAME[], int REMOTE_PORT);

// Creates an new socket 
int socketCreate();

// Initializes Host information
void hostCreate(struct sockaddr_in* hostAddr, int localPort);

// Initializes Client information
void clientCreate(struct sockaddr_in* clientAddr, char hostName[], int portNum, struct hostent** Client_Info);

// Bind host to socket
void bindInitialization(int socket, struct sockaddr_in* hostAddr, int len);

// Send messages
int sendMsg(int socket, char* msg, int msgLen, struct sockaddr_in* clientAddr,int size);


// Receive messages
int receiveMsg(int socket, char* buffer, int bufferLen, struct sockaddr_in* clientAddr,int* size);
