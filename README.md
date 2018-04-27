# LocalChatClient
Simple chat client that lets you communicate between 2 computers on the same network

The client uses UDP sockets to communicate between computers on a specified port

Requirments:
Main requires 3 arguments specified by the user:
- local port number
- remote machine name
- remote port number

To use, complie the app using make and run the command:
./s-talk {local port number} {remote machine name} {remote port number}

Ex.
./s-talk 1000 localhost 2000
