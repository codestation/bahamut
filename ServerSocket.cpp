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