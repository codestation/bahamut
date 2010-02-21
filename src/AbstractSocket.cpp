/*
 * AbstractSocket.cpp
 *
 *  Created on: 02/02/2010
 *      Author: code
 */

#include "AbstractSocket.h"

#ifdef _WIN32
bool AbstractSocket::init = false;
#endif


AbstractSocket::AbstractSocket() {
	sock = 0;
	proto = 0;
	port = 0;
}

#ifdef _WIN32
/*
 * Socket initializer (must be called before any socket function)
 * Parameters:
 * 		none
 * Returns: true if u can use the socket function, false otherwise
*/
bool Socket::WSAStart() {
	if(!init) {
		WSADATA info;
		if(!WSAStartup(MAKEWORD(2,2), &info))
			init = true;
	}
	return init;
}

/*
 * Socket cleaner (must be called when finish to use the socket functions)
 * Note: must be called for every time that WSAStartup was called
 * Parameters:
 * 		none
 * Returns: void
*/
void Socket::WSAClean() {
	if(init) {
		WSACleanup();
		init = false;
	}
}
#endif

ssize_t AbstractSocket::receiveData(char *buffer, size_t size) {
	return recv(sock, buffer, size, 0);
}

ssize_t AbstractSocket::receiveData(Packet *packet) {
	return recv(sock, (char *)packet->getData(), packet->getMaxPacketSize(), 0);
}

ssize_t AbstractSocket::receiveData(Packet *packet, ClientInfo *info) {
	socklen_t addrlen = info->getSocketSize();
	return recvfrom( sock, (char *)packet->getData(), packet->getMaxPacketSize(), 0, (struct sockaddr *)info->getSocketInfo(), &addrlen);
}

ssize_t AbstractSocket::sendData(const char *data, size_t length) {
	return send(sock, data, length , 0);
}

ssize_t AbstractSocket::sendData(Packet *packet) {
	return send(sock, (char *)packet->getData(), packet->getSize() , 0);
}

ssize_t AbstractSocket::sendData(Packet *packet, ClientInfo *info) {
	return sendto( sock, (char *)packet->getData(), packet->getSize(), 0, (struct sockaddr *)info->getSocketInfo(),info->getSocketSize());
}

void AbstractSocket::closeSocket() {
	if(sock) {
#ifdef _WIN32
		closesocket(sock);
		WSAClean();
#else
		close(sock);
#endif
		sock = 0;
	}
}

AbstractSocket::~AbstractSocket() {
	// TODO Auto-generated destructor stub
}
