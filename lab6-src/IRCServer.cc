
const char * usage =
"                                                               \n"
"IRCServer:                                                   \n"
"                                                               \n"
"Simple server program used to communicate multiple users       \n"
"                                                               \n"
"To use it in one window type:                                  \n"
"                                                               \n"
"   IRCServer <port>                                          \n"
"                                                               \n"
"Where 1024 < port < 65536.                                     \n"
"                                                               \n"
"In another window type:                                        \n"
"                                                               \n"
"   telnet <host> <port>                                        \n"
"                                                               \n"
"where <host> is the name of the machine where talk-server      \n"
"is running. <port> is the port number you used when you run    \n"
"daytime-server.                                                \n"
"                                                               \n";

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include "LinkedList.h"
#include "IRCServer.h"

int QueueLength = 5;


int
IRCServer::open_server_socket(int port) {

	// Set the IP address and port for this server
	struct sockaddr_in serverIPAddress; 
	memset( &serverIPAddress, 0, sizeof(serverIPAddress) );
	serverIPAddress.sin_family = AF_INET;
	serverIPAddress.sin_addr.s_addr = INADDR_ANY;
	serverIPAddress.sin_port = htons((u_short) port);
  
	// Allocate a socket
	int masterSocket =  socket(PF_INET, SOCK_STREAM, 0);
	if ( masterSocket < 0) {
		perror("socket");
		exit( -1 );
	}

	// Set socket options to reuse port. Otherwise we will
	// have to wait about 2 minutes before reusing the sae port number
	int optval = 1; 
	int err = setsockopt(masterSocket, SOL_SOCKET, SO_REUSEADDR, 
			     (char *) &optval, sizeof( int ) );
	
	// Bind the socket to the IP address and port
	int error = bind( masterSocket,
			  (struct sockaddr *)&serverIPAddress,
			  sizeof(serverIPAddress) );
	if ( error ) {
		perror("bind");
		exit( -1 );
	}
	
	// Put socket in listening mode and set the 
	// size of the queue of unprocessed connections
	error = listen( masterSocket, QueueLength);
	if ( error ) {
		perror("listen");
		exit( -1 );
	}

	return masterSocket;
}

void
IRCServer::runServer(int port)
{
	int masterSocket = open_server_socket(port);

	initialize();
	
	while ( 1 ) {
		
		// Accept incoming connections
		struct sockaddr_in clientIPAddress;
		int alen = sizeof( clientIPAddress );
		int slaveSocket = accept( masterSocket,
					  (struct sockaddr *)&clientIPAddress,
					  (socklen_t*)&alen);
		
		if ( slaveSocket < 0 ) {
			perror( "accept" );
			exit( -1 );
		}
		
		// Process request.
		processRequest( slaveSocket );		
	}
}

int
main( int argc, char ** argv )
{
	// Print usage if not enough arguments
	if ( argc < 2 ) {
		fprintf( stderr, "%s", usage );
		exit( -1 );
	}
	
	// Get the port from the arguments
	int port = atoi( argv[1] );

	IRCServer ircServer;

	// It will never return
	ircServer.runServer(port);
	
}

//
// Commands:
//   Commands are started y the client.
//
//   Request: ADD-USER <USER> <PASSWD>\r\n
//   Answer: OK\r\n or DENIED\r\n
//
//   REQUEST: GET-ALL-USERS <USER> <PASSWD>\r\n
//   Answer: USER1\r\n
//            USER2\r\n
//            ...
//            \r\n
//
//   REQUEST: CREATE-ROOM <USER> <PASSWD> <ROOM>\r\n
//   Answer: OK\n or DENIED\r\n
//
//   Request: LIST-ROOMS <USER> <PASSWD>\r\n
//   Answer: room1\r\n
//           room2\r\n
//           ...
//           \r\n
//
//   Request: ENTER-ROOM <USER> <PASSWD> <ROOM>\r\n
//   Answer: OK\n or DENIED\r\n
//
//   Request: LEAVE-ROOM <USER> <PASSWD>\r\n
//   Answer: OK\n or DENIED\r\n
//
//   Request: SEND-MESSAGE <USER> <PASSWD> <MESSAGE> <ROOM>\n
//   Answer: OK\n or DENIED\n
//
//   Request: GET-MESSAGES <USER> <PASSWD> <LAST-MESSAGE-NUM> <ROOM>\r\n
//   Answer: MSGNUM1 USER1 MESSAGE1\r\n
//           MSGNUM2 USER2 MESSAGE2\r\n
//           MSGNUM3 USER2 MESSAGE2\r\n
//           ...\r\n
//           \r\n
//
//    REQUEST: GET-USERS-IN-ROOM <USER> <PASSWD> <ROOM>\r\n
//    Answer: USER1\r\n
//            USER2\r\n
//            ...
//            \r\n
//

void
IRCServer::processRequest( int fd )
{
	// Buffer used to store the comand received from the client
	const int MaxCommandLine = 1024;
	char commandLine[ MaxCommandLine + 1 ];
	int commandLineLength = 0;
	int n;
	
	// Currently character read
	unsigned char prevChar = 0;
	unsigned char newChar = 0;
	
	//
	// The client should send COMMAND-LINE\n
	// Read the name of the client character by character until a
	// \n is found.
	//

	// Read character by character until a \n is found or the command string is full.
	while ( commandLineLength < MaxCommandLine &&
		read( fd, &newChar, 1) > 0 ) {

		if (newChar == '\n' && prevChar == '\r') {
			break;
		}
		
		commandLine[ commandLineLength ] = newChar;
		commandLineLength++;

		prevChar = newChar;
	}
	
	// Add null character at the end of the string
	// Eliminate last \r
	commandLineLength--;
        commandLine[ commandLineLength ] = 0;

	printf("RECEIVED: %s\n", commandLine);

	char * command = (char *) malloc(100 * sizeof(char));
	char * user = (char *) malloc(100 * sizeof(char));
	char * password = (char *) malloc(100 * sizeof(char));
	char * args = (char *) malloc(100 * sizeof(char));
	char * message = (char *)malloc(150 * sizeof(char));

	int argc = 0;
	int i = 0;   //character in commandLine
	int j = 0;   //character in each array

	for (i = 0; i < 100; i++) {
		if (commandLine[i] == ' ' && argc != 4) {
			argc++;
			j = 0;
		}
		else if (argc == 0)   //command
			command[j++] = commandLine[i];
		else if (argc == 1)   //user name
			user[j++] = commandLine[i];
		else if (argc == 2)   //password
			password[j++] = commandLine[i];
		else if (argc == 3)   //argument
			args[j++] = commandLine[i];
		else if (argc == 4)
			message[j++] = commandLine[i];
		else
			break;
	}

	if (args[0] == '\0')
		args = strdup("NULL");
	if (message[0] == '\0')
		message = strdup("NULL");


	// printf("The commandLine has the following format:\n");
	// printf("COMMAND <user> <password> <arguments>. See below.\n");
	// printf("You need to separate the commandLine into those components\n");
	// printf("For now, command, user, and password are hardwired.\n");


	printf("command=%s\n", command);
	printf("user=%s\n", user);
	printf( "password=%s\n", password );
	printf("args=%s\n", args);

	if (!strcmp(command, "ADD-USER")) {
		addUser(fd, user, password, args);
	}
	else if (!strcmp(command, "CREATE-ROOM")) {
		createRoom(fd, user, password, args);
	}
	else if (!strcmp(command, "LIST-ROOMS")) {
		listRoom(fd, user, password, args);
	}
	else if (!strcmp(command, "ENTER-ROOM")) {
		enterRoom(fd, user, password, args);
	}
	else if (!strcmp(command, "LEAVE-ROOM")) {
		leaveRoom(fd, user, password, args);
	}
	else if (!strcmp(command, "SEND-MESSAGE")) {
		sendMessage(fd, user, password, args, message);
	}
	else if (!strcmp(command, "GET-MESSAGES")) {
		getMessages(fd, user, password, args, message);
	}
	else if (!strcmp(command, "GET-USERS-IN-ROOM")) {
		getUsersInRoom(fd, user, password, args);
	}
	else if (!strcmp(command, "GET-ALL-USERS")) {
		getAllUsers(fd, user, password, args);
	}
	else {
		const char * msg =  "UNKNOWN COMMAND\r\n";
		write(fd, msg, strlen(msg));
	}

	// Send OK answer
	//const char * msg =  "OK\n";
	//write(fd, msg, strlen(msg));

	close(fd);	
}


typedef struct MESSAGE {
	const char * userName;
	const char * messageContent;
} MESSAGE;

typedef struct ROOM {   //all room
	const char * roomName;
	int userCount;
	int messageCount;
	LinkedList * usersInRoom;
	MESSAGE * messageList;
} ROOM;


#define MAX_ROOM_COUNT 50   //limit 50 rooms per server
#define MAX_MESSAGE_COUNT 100   //limit 100 messages per room 
#define ROOM_NAME_FILE "roomName.txt"
LinkedList * usersTotal = (LinkedList *)malloc(300 * sizeof(ListNode));
LinkedList * roomNameExisted = (LinkedList *)malloc(50 * sizeof(ListNode));
ROOM roomList[MAX_ROOM_COUNT];     //limit 50 rooms in this server
//MESSAGE message[MAX_MESSAGE_COUNT];   //limit 100 message per room

int roomNumber = 0;
int messageNumber = 0;

void
IRCServer::initialize()
{
	// Open password file
	FILE *fd;
	fd = fopen(PASSWORD_FILE, "w+");
	FILE *file;
	file = fopen(ROOM_NAME_FILE, "w+");
	// Initialize users in room
	llist_init(usersTotal);
	llist_init(roomNameExisted);
	
	int i;
	int j;
	for (i = 0; i < MAX_ROOM_COUNT; i++) {
		roomList[i].roomName = (char *)malloc(100 * sizeof(char));   //room name nolonger than 100 characters
		roomList[i].userCount = 0;
		roomList[i].messageCount = 0;
		roomList[i].usersInRoom = (LinkedList *)malloc(50 * sizeof(ListNode));  //userCount no more than 50 people
		roomList[i].messageList = (MESSAGE *)malloc(100 * sizeof(MESSAGE));
		for (j = 0; j < MAX_MESSAGE_COUNT; j++) {
			roomList[i].messageList[j].userName = (char *)malloc(50 * sizeof(char));
			roomList[i].messageList[j].messageContent = (char *)malloc(250 * sizeof(char));
		}
	}

	// roomList[0].roomName = strdup("Default Room");   //create a default char room "Default Room"
	// llist_add_room(roomNameExisted, roomList[0].roomName, roomNumber);
	// llist_save(roomNameExisted, (char *)ROOM_NAME_FILE);
	// roomNumber++;
	//Initalize message list
}

bool
IRCServer::checkPassword(int fd, const char * user, const char * password) {
	// Here check the password
	if (llist_exists_name(usersTotal, user) == 0){
		return false;
	}
	else if (strcmp(llist_password(usersTotal, user), password) == 0) {
		return true;
	}
	else 
		return false;;
}

void
IRCServer::addUser(int fd, const char * user, const char * password, const char * args)
{
	// Here add a new user. For now always return OK.
	if (llist_exists_name(usersTotal, user) == 0) {   //return 0 is not exist
		llist_add(usersTotal, user, password);	
		llist_save(usersTotal, (char *)PASSWORD_FILE);
		const char * msg =  "OK\r\n";
		write(fd, msg, strlen(msg));
	}
	else {   //return 1 is exists
		const char * msg =  "DENIED\r\n";
		write(fd, msg, strlen(msg));
	}

	return;		
}

void
IRCServer::createRoom(int fd, const char * user, const char * password, const char * args)
{	
	if (checkPassword(fd, user, password)) {
		if (llist_exists_name(roomNameExisted, args) == 0) {
			roomList[roomNumber].roomName = strdup(args);    //take args as the room name
			//store room name and number into inkedlist
			llist_add_room(roomNameExisted, roomList[roomNumber].roomName, roomNumber);  
			llist_save(roomNameExisted, (char *)ROOM_NAME_FILE);
			roomNumber++;

			const char * msg =  "OK\r\n";
			write(fd, msg, strlen(msg));
		}
		else {
			const char * msg =  "DENIED\r\n";
			write(fd, msg, strlen(msg));
		}
	}
	else {
		const char * msg = "ERROR (Wrong password)\r\n";
		//const char * msg = "DENIED\r\n";
		write(fd,msg,strlen(msg));
	}

	return;
}

void
IRCServer::listRoom(int fd, const char * user, const char * password, const char * args)
{
	if (checkPassword(fd,user,password)) {
		if (roomNumber != 0) {
			llist_sort_room(roomNameExisted);
			ListNode * e;
			e = roomNameExisted->head;
			while (e != NULL) {
				write(fd, e->userName, strlen(e->userName));
				write(fd,"\r\n",2);
				e = e->next;
			}
			write(fd,"\r\n",2);
			close(fd);
		}
		else {
			const char * msg = "No Room Existed\r\n";
			write(fd,msg,strlen(msg));
		}
	}
	else {
		const char * msg = "ERROR (Wrong password)\r\n";
		//const char * msg = "DENIED\r\n";
		write(fd,msg,strlen(msg));
	}
}

void
IRCServer::enterRoom(int fd, const char * user, const char * password, const char * args)
{
	int roomNum;
	if (checkPassword(fd, user,password)) {
		if (llist_exists_name(roomNameExisted, args) == 0) {   //args as room name is not exist
			const char * msg = "ERROR (No room)\r\n";
			//const char * msg = "DENIED\r\n";
			write(fd,msg,strlen(msg));
		}
		else {
			roomNum = llist_roomNumber(roomNameExisted, args);  //check the room number
			if (roomList[roomNum].userCount < 50){
				if (llist_exists_name(roomList[roomNum].usersInRoom, user) == 0) {
					roomList[roomNum].userCount++;
					llist_add(roomList[roomNum].usersInRoom, user, password);
				}
				const char * msg =  "OK\r\n";
				write(fd, msg, strlen(msg));
			}
			else {
				//const char * msg = "ERROR (Room is FULL)\r\n";
				const char * msg = "DENIED\r\n";
				write(fd,msg,strlen(msg));
			}
		}
	}
	else {
		const char * msg = "ERROR (Wrong password)\r\n";
		//const char * msg = "DENIED\r\n";
		write(fd,msg,strlen(msg));
	}
}

void
IRCServer::leaveRoom(int fd, const char * user, const char * password, const char * args)
{
	int roomNum;
	int i;
	if (checkPassword(fd, user,password)) {
		if (llist_exists_name(roomNameExisted, args) == 0) {   //args as room name is not exist
			//const char * msg = "ERROR (Room not exist)\r\n";
			const char * msg = "DENIED\r\n";
			write(fd,msg,strlen(msg));
		}
		else {
			roomNum = llist_roomNumber(roomNameExisted, args);  //check the room number
			if (llist_exists_name(roomList[roomNum].usersInRoom, user) == 1){
				llist_remove(roomList[roomNum].usersInRoom, user);
				// for (i = 0; i < messageNumber; i++) {
				// 	if (strcmp(roomList[roomNum].messageList[i].userName, user) == 0) {
				// 		//llist_remove(roomList[roomNum].messageList,roomList[roomNum].messageList[i]);
				// 		i = i + 1;
				// 		//messageNumber--;
				// 		roomList[roomNum].messageCount--;
				// 	}
				// }
				roomList[roomNum].userCount--;
				const char * msg =  "OK\r\n";
				write(fd, msg, strlen(msg));
			}
			else {
				//const char * msg = "ERROR (Room is FULL)\r\n";
				const char * msg = "ERROR (No user in room)\r\n";
				write(fd,msg,strlen(msg));
			}
		}
	}
	else {
		const char * msg = "ERROR (Wrong password)\r\n";
		//const char * msg = "DENIED\r\n";
		write(fd,msg,strlen(msg));
	}
}

void
IRCServer::sendMessage(int fd, const char * user, const char * password, const char * args, char * message)
{
	int roomNum;
	strcat(message, "\r\n");
	// if (message = NULL) {
	// 	const char * msg = "ERROR (Wrong password)\r\n";
	// 	//const char * msg = "DENIED\r\n";
	// 	write(fd,msg,strlen(msg));
	// }
	if (checkPassword(fd, user, password)) {
		roomNum = llist_roomNumber(roomNameExisted, args);  //check the room number
		if (llist_exists_name(roomList[roomNum].usersInRoom, user) == 1){
			roomList[roomNum].messageList[roomList[roomNum].messageCount].userName = strdup(user);
			roomList[roomNum].messageList[roomList[roomNum].messageCount].messageContent = strdup(message);
			roomList[roomNum].messageCount++;
			//messageNumber++;
			//if (messageNumber == 100)
			//	messageNumber = 0;
			if (roomList[roomNum].messageCount == 100)
				roomList[roomNum].messageCount = 0;
			const char * msg = "OK\r\n";
			write(fd, msg, strlen(msg));
		}
		else {
			const char * msg = "ERROR (user not in room)\r\n";
			//const char * msg = "DENIED\r\n";
			write(fd,msg,strlen(msg));
		}
	}
	else {
		const char * msg = "ERROR (Wrong password)\r\n";
		//const char * msg = "DENIED\r\n";
		write(fd,msg,strlen(msg));
	}
}

void
IRCServer::getMessages(int fd, const char * user, const char * password, char * args, const char * roomName)
{
	int roomNum;
	int messageNum = atoi(args);
	if (checkPassword(fd, user, password)) {
		if (llist_exists_name(roomNameExisted, roomName) == 1) {
			roomNum = llist_roomNumber(roomNameExisted, roomName);  //check the room number
			if (llist_exists_name(roomList[roomNum].usersInRoom, user) == 1){
				//if (messageNum <= messageNumber) {
				if (messageNum < roomList[roomNum].messageCount){
					int mess;
					for (mess = messageNum; mess < roomList[roomNum].messageCount; mess++) {
						char number[10];
						//sprintf(msg, "%d", mess);
						snprintf(number, sizeof(number), "%i", mess);
						if (mess < 10) 
							write(fd, number, 1);
						else
							write(fd, number, 2);
						write(fd, " ", 1);
						write(fd, roomList[roomNum].messageList[mess].userName, 
							strlen(roomList[roomNum].messageList[mess].userName));
						write(fd, " ", 1);	
						write(fd, roomList[roomNum].messageList[mess].messageContent, 
							strlen(roomList[roomNum].messageList[mess].messageContent));
						//msg = "";
					}
					write(fd,"\r\n", sizeof("\r\n"));
				}
				else {
					const char * msg = "NO-NEW-MESSAGES\r\n";
					write(fd,msg,strlen(msg));
				}
			}
			else {
				const char * msg = "ERROR (User not in room)\r\n";
				//const char * msg = "DENIED\r\n";
				write(fd,msg,strlen(msg));
			}
		}
	}
	else {
		const char * msg = "ERROR (Wrong password)\r\n";
		//const char * msg = "DENIED\r\n";
		write(fd,msg,strlen(msg));
	}
}

void
IRCServer::getUsersInRoom(int fd, const char * user, const char * password, const char * args)
{
	int roomNum;
	if (checkPassword(fd,user,password)) {
		if (llist_exists_name(roomNameExisted, args) == 0) {   //args as room name is not exist
			//const char * msg = "ERROR (Room not exist)\r\n";
			const char * msg = "DENIED\r\n";
			write(fd,msg,strlen(msg));
		}
		else {
			roomNum = llist_roomNumber(roomNameExisted, args);
			if (roomList[roomNum].userCount != 0) {
				llist_sort(roomList[roomNum].usersInRoom);
				ListNode * e;
				e = roomList[roomNum].usersInRoom->head;
				while (e != NULL) {
					write(fd, e->userName, strlen(e->userName));
					write(fd,"\r\n",2);
					e = e->next;
				}
			}
			write(fd,"\r\n",2);
			close(fd);
		}
	}
	else {
		const char * msg = "ERROR (Wrong password)\r\n";
		//const char * msg = "DENIED\r\n";
		write(fd,msg,strlen(msg));
	}
}

void
IRCServer::getAllUsers(int fd, const char * user, const char * password,const  char * args)
{
	if (checkPassword(fd,user,password)) {
		llist_sort(usersTotal);
		ListNode * e;
		e = usersTotal->head;
		while (e != NULL) {
			write(fd, e->userName, strlen(e->userName));
			write(fd,"\r\n",2);
			e = e->next;
		}
		write(fd,"\r\n",2);
		close(fd);
	}
	else {
		const char * msg = "ERROR (Wrong password)\r\n";
		//const char * msg = "DENIED\r\n";
		write(fd,msg,strlen(msg));
	}
}


