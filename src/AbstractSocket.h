/*
 * AbstractSocket.h
 *
 *  Created on: 02/02/2010
 *      Author: code
 */

#ifndef ABSTRACTSOCKET_H_
#define ABSTRACTSOCKET_H_

#ifdef _WIN32
#define WINVER 0x0501 //Windows XP
#include <windows.h>
#include <winsock2.h>
#else
#include <unistd.h>
#include <netdb.h>
#include <netinet/in.h>
#include <errno.h>
#endif
#include "Packet.h"
#include "ClientInfo.h"


class AbstractSocket {
private:
	int sock;
	int port;
	int proto;
#ifdef _WIN32
	static bool init;
#endif
public:
	enum socket_type {TCP_SOCKET, UDP_SOCKET};

	AbstractSocket(int port, socket_type proto);
#ifdef _WIN32
	bool Socket::WSAStart();
	void Socket::WSAClean();
#endif
	ssize_t receiveData(char *buffer, size_t size);
	ssize_t receiveData(Packet *packet);
	ssize_t receiveData(Packet *packet, ClientInfo *info);
	ssize_t sendData(const char *data, size_t length);
	ssize_t sendData(Packet *packet);
	ssize_t sendData(Packet *packet, ClientInfo *info);
	void closeSocket();
	virtual ~AbstractSocket();
};

#endif /* ABSTRACTSOCKET_H_ */
