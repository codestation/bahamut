/*
 *  Project Bahamut: full ad-hoc tunneling software to be used by the
 *  Playstation Portable (PSP) to emulate online features.
 *
 *  Copyright (C) 2008-2010  Codestation
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

#include "Socket.h"

#ifdef _WIN32
bool Socket::init = false;
#endif

Socket::Socket(int sock, sockaddr_in *addr) {
	this->sock = sock;
	memcpy(&client, addr, sizeof(client));
	port = 0;
}

Socket::Socket(const char *addr, int port) {
	sock = 0;
	strcpy(host, addr);
	this->port = port;
}

bool Socket::connectSocket(socket_type proto) {
#ifdef _WIN32
	if(!WSAStart())
		return false;
#endif
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

Socket::~Socket() {

}
