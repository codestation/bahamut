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
 *     TCP/UDP Server functions
 * Special Notes:
 *     TODO: finish the TCP implementation
 *     		 Posible merge with Socket class
 */

#include "ServerSocket.h"

#ifdef _WIN32
bool ServerSocket::init = false;
#endif

ServerSocket::ServerSocket(int port, socket_type proto) {
	sock = 0;
	this->proto = proto;
	this->port = port;
#ifdef _WIN32
	lpMsgBuf = 0;
#endif
}

#ifdef _WIN32
/*
 * Socket initializer (must be called before any socket function)
 * Parameters:
 * 		none
 * Returns: true if u can use the socket function, false otherwise
*/
bool ServerSocket::WSAStart() {
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
void ServerSocket::WSAClean() {
	if(init) {
		WSACleanup();
		init = false;
	}
}
#endif


bool ServerSocket::bindSocket() {
#ifdef _WIN32
	if(!WSAStart())
		return false;
#endif
	if((sock = socket(PF_INET, proto == TCP_SOCKET ? SOCK_STREAM : SOCK_DGRAM, 0)) < 0)
		return false;
#ifdef _WIN32
	int iOptVal = 2;
	setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (char*)&iOptVal, sizeof(timeval));
#else
	struct timeval tv;
	tv.tv_sec = 2;
	tv.tv_usec = 0;
	setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(timeval));
#endif
	sockaddr_in server;
	memset(&server, 0, sizeof(server));
	server.sin_family = AF_INET;
	server.sin_port = htons(port);
	server.sin_addr.s_addr = htonl (INADDR_ANY);
	return bind( sock, (struct sockaddr *)&server, sizeof(server)) == 0;
}

bool ServerSocket::listenConnection(int max) {
	return listen( sock, max) != -1;
}

Socket * ServerSocket::acceptConnection() {
	sockaddr_in data;
	Socket *newsock = NULL;
	socklen_t len = sizeof(data);
	int res = accept( sock, (sockaddr *)&data, &len);
	if(res >= 0)
		newsock = new Socket(res, &data);
	return newsock;
}
/*
bool ServerSocket::readAvailable() {
	timeout.tv_sec = 1;
	timeout.tv_usec = 0;
	if(select(-1, &readfds, NULL, NULL, &timeout) < 0)
		return false;
	else
		return FD_ISSET(sock, &readfds);
}
*/
int ServerSocket::receive(Packet *packet, ClientInfo *info) {
	socklen_t addrlen = info->getSocketSize();
	int res = recvfrom( sock, (char *)packet->getData(), packet->getMaxPacketSize(), 0, (struct sockaddr *)info->getSocketInfo(), &addrlen);
	return res;
}

int ServerSocket::send(Packet *packet, ClientInfo *info) {
	return sendto( sock, (char *)packet->getData(), packet->getSize(), 0, (struct sockaddr *)info->getSocketInfo(),info->getSocketSize());
}

void ServerSocket::closeSocket() {
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

ServerSocket::~ServerSocket() {
#ifdef _WIN32
	if(lpMsgBuf)
		LocalFree(lpMsgBuf);
#endif
}
