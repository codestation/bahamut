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
*	TCP/UDP Socket class
*Special Notes:
*	none yet
*
*Copyright Stuff:
* 	TODO: put GPLv3 header
*/


#ifndef SOCKET_H_
#define SOCKET_H_

#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <string.h>

#define PACKET_HEADER 10

class Socket
{
private:
	int port;
	int sock;
	int socket_flag;
	char *host;
	int proto;
	u_int packet_count;
	sockaddr_in client;

	in_addr_t resolve(const char *url);
public:
	Socket(const char *addr, int port, const char *proto);
	Socket(int s);
	virtual ~Socket();
	bool connectSocket();
	ssize_t readSocket(void *, size_t size);
	ssize_t writeSocket(const void *, size_t size);
	void closeSocket();
};

#endif /*SOCKET_H_*/
