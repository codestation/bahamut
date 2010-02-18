/*
 *  Project Bahamut: full ad-hoc tunneling software to be used by the
 *  Playstation Portable (PSP) to emulate online features.
 *
 *  Copyright (C) 2008  Codestation
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/*
 * File Description:
 *     TCP/UDP Socket class
 * Special Notes:
 *     TODO: change the "proto" param from char* to int
 */

#include "Socket.h"

#ifdef _WIN32
bool Socket::init = false;
#endif

/*
 * Socket constructor
 * Parameters:
 * 		addr: IP address or hostname
 * 		port: port number to connect
 * 		proto: "tcp" or "udp"
 * Returns: void
*/
Socket::Socket(const char *addr, int port, socket_type proto) {
	sock = 0;
	strcpy(host, addr);
	this->proto = proto;
	this->port = port;
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


/*
 * Socket constructor
 * Parameters:
 * 		sock: a socket handler
 * Returns: void
*/
Socket::Socket(int sock, sockaddr_in *addr) {
	this->sock = sock;
	memcpy(&client, addr, sizeof(client));
	/*
#ifdef _WIN32
		int iOptVal = 2;
		setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (char*)&iOptVal, sizeof(timeval));
#else
		struct timeval tv;
		tv.tv_sec = 2;
		tv.tv_usec = 0;
		setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(timeval));
#endif*/
}

/*
 * Tries to create and connect to the socket
 * Parameters:
 * 		none
 * Returns: true on sucess, false otherwise
*/
bool Socket::connectSocket() {
	if((sock = socket(PF_INET, proto == TCP_SOCKET ? SOCK_STREAM : SOCK_DGRAM, 0)) >= 0) {
#ifdef _WIN32
		int iOptVal = 2;
		setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (char*)&iOptVal, sizeof(timeval));
#else
		struct timeval tv;
		tv.tv_sec = 2;
		tv.tv_usec = 0;
		setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(timeval));
#endif
		memset(&client, 0, sizeof(client));
		client.sin_family = AF_INET;
		client.sin_port = htons(port);
		hostent *host_n = (hostent *)gethostbyname(host);
		if(host_n == NULL)
			return false;
		client.sin_addr.s_addr = ((in_addr*)(host_n->h_addr))->s_addr;
		return connect(sock, (struct sockaddr *)&client, sizeof(client)) >= 0;
	} else {
		return false;
	}
}

/*
 * Reads a socket and store the data in the specified buffer
 * The function doesnt return until some bytes are readed or an error
 * ocuur
 * Parameters:
 * 		buffer: pointer to a buffer that will hold the data
 * 		size: max bytes to be read by the socket
 * Returns: (>= 0) number of bytes received, -1 on error
*/
ssize_t Socket::readSocket(char *buffer, size_t size) {
	return recv(sock, buffer, size, 0);
}

/*
 * Reads a socket and store the data in the specified buffer
 * The function doesnt return until some bytes are readed or an error
 * ocuur
 * Parameters:
 * 		packet: object who holds the data readed by the socket
 * Returns: (>= 0) number of bytes received, -1 on error
*/
ssize_t Socket::readSocket(Packet *packet) {
	return recv(sock, (char *)packet->getData(), packet->getMaxPacketSize(), 0);
}

/*
 * Writes the specified data into a socket
 * The function doesnt return until all the buffer is written or an error
 * ocuur
 * Parameters:
 * 		data: pointer to a buffer that will hold the data
 * 		size: max bytes to be written to the socket
 * Returns: (>= 0) number of bytes written, -1 on error
*/
ssize_t Socket::writeSocket(const char *data, size_t length) {
	return send(sock, data, length , 0);
}

/*
 * Writes the specified data into a socket
 * The function doesnt return until all the buffer is written or an error
 * ocuur
 * Parameters:
 * 		packet: object who holds the data to be written to the socket
 * Returns: (>= 0) number of bytes written, -1 on error
*/
ssize_t Socket::writeSocket(Packet *packet) {
	return send(sock, (char *)packet->getData(), packet->getSize() , 0);
}

/*
 * Closes the socket
 * Parameters:
 * 		none
 * Returns: void
*/
void Socket::closeSocket() {
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

/*
 * Default destructor
 * Parameters:
 * 		none
 * Returns: void
*/
Socket::~Socket() {
	//make sure that we close the socket before destroy the object
	closeSocket();
}
