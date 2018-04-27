/* threading.h
 *
 * Description:
 * Sets up thread functions for chat client
 * Allows user to send and recieve messages concurrently
 *
 * Author: Jorawar Chana
 * Data Last Modified: Feb. 10, 2018
 *
 */


#define ESCAPE_CHAR "!\n\0"
#define NUM_THREADS_NET 2
#define NUM_THREADS_IO 2


pthread_mutex_t message_mutex;
pthread_cond_t messageRecieved_cond;
pthread_cond_t messageReadyToOutput_cond;

pthread_mutex_t sent_mutex;
pthread_cond_t messageReadyToSend_cond;


// struct to transfer required sender and reciever data
struct senderAndReciever_thread{
	int socket;
	char* msg;
	int msgLen;
	struct sockaddr_in* clientAddr;
	int size;
	LIST* sharedData;
};

struct monitorIO_thread{
	LIST* list;	// Stores messages
};

// -- Getters -- //

// Returns ID to threads
pthread_t* getThreads();

// Returns Sender thread ID
int getSenderID();

// Returns Reciever thread ID
int getRecieverID();

// Returns Keyboard thread ID
int getKeyboardID();

// Returns Monitor thread ID
int getMonitorID();


// -- Functions control thread logic -- //

// Manage sender thread
// Data should be a pointer to a senderAndReciever object 
void* send_thread(void* data);


// Manage reciever thread
// Data should be a pointer to a senderAndReciever object 
void* recv_thread(void* data);


// Manage keyboard thread
// Recieved data should be stored in a global List object
// Critical Section for List object in send thread and keyboard monitor thread
void* keyboardInput_thread(void* data);


// Manage monitor thread
// Recieved data should be stored in a global List object
// Critical Section for List object in send thread and keyboard monitor thread
void* monitorOutput_thread(void* data);
