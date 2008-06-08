/*
*Project Name: Bahamut
* 
*Programmers: Codestation, ZackVixACD
* 
*Project Descrption: The project bahamut is a full ad-hoc tunneling
*	software to be used by the Playstation Portable (PSP) to emulate 
*	online features.
*
*File Description:
*	TCP server
*Special Notes:
*	TODO: incomplete class
*
*Copyright Stuff:
* 	TODO: put GPLv3 header
*/

#include "ServerSocket.h"

ServerSocket::ServerSocket()
{
}

ServerSocket::~ServerSocket()
{
}

bool Socket::bindSocket() {
	client.sin_family = AF_INET;
	client.sin_port = htons(port);	
	client.sin_len = sizeof(client);
	client.sin_addr.s_addr = htonl (INADDR_ANY);
	return bind( sock, (struct sockaddr *)&client, sizeof(client)) != -1;
}

bool Socket::listenSocket(int max) {
	return listen( sock, max) != -1;
}

Socket *Socket::acceptSocket() {
	int s = accept( sock, (struct sockaddr *)&client, sizeof(client));
	return s != NULL ? new Socket(s) : NULL;
}