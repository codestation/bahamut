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
*	TODO: remove some specific lines of writeSocket
* 		: change the proto variable from char * to int (to get rid of strdup)
*
*Copyright Stuff:
*   This program is free software: you can redistribute it and/or modify
*   it under the terms of the GNU General Public License as published by
*   the Free Software Foundation, either version 3 of the License, or
*   (at your option) any later version.
* 
*   This program is distributed in the hope that it will be useful,
*   but WITHOUT ANY WARRANTY; without even the implied warranty of
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*   GNU General Public License for more details.
* 
*  You should have received a copy of the GNU General Public License
*   along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "Socket.h"

/*
 * Socket constructor
 * Parameters:
 * 		addr: IP address or hostname
 * 		port: port number to connect
 * 		proto: "tcp" or "udp"
 * Returns: void
*/
Socket::Socket(const char *addr, int port, const char *proto) {
	sock = 0;
	socket_flag = 1;
	packet_count = 0;
	host = strdup(addr);
	if(strcmp(proto,"tcp") == 0) {
		this->proto = IPPROTO_TCP;
	} else if (strcmp(proto,"udp") == 0) {
		this->proto = IPPROTO_UDP;
	} else {
		this->proto = -1;
	}
	this->port = port;
}

/*
 * Socket constructor
 * Parameters:
 * 		sock: a socket handler
 * Returns: void
*/
Socket::Socket(int sock) {
	this->sock = sock;
}

/*
 * Tries to create and connect to the socket
 * Parameters:
 * 		none
 * Returns: true on sucess, false otherwise
*/
bool Socket::connectSocket() {
	if((sock = socket(PF_INET, proto == IPPROTO_TCP ? SOCK_STREAM : SOCK_DGRAM, proto)) >= 0) {
		memset(&client, 0, sizeof(client));
		client.sin_family = AF_INET;
		client.sin_port = htons(port);
		client.sin_addr.s_addr = ((in_addr*)((hostent *)gethostbyname(host)->h_addr))->s_addr;
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
ssize_t Socket::readSocket(void *buffer, size_t size) {
	return recv(sock, buffer, size, 0);
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
ssize_t Socket::readSocket(PspPacket *packet) {
	int res = recv(sock, packet->getPacketData(), packet->getPacketSize(), 0);
	if(res >= 0) {
		packet->setPayloadSize(res);
	}
	return res;
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
ssize_t Socket::writeSocket(const void *data, size_t length) {
	return send(sock, data, length , 0);
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
ssize_t Socket::writeSocket(PspPacket *packet) {
	return send(sock, packet->getPacketData(), packet->getPacketSize() , 0);
}

/*
 * Closes the socket
 * Parameters:
 * 		none
 * Returns: void
*/
void Socket::closeSocket() {
	if(sock) {
		close(sock);
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
