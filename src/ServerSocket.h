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

#ifndef SERVERSOCKET_H_
#define SERVERSOCKET_H_

#ifdef _WIN32
#include <windows.h>
#include <winsock2.h>
#else
#include <unistd.h>
#include <netdb.h>
#include <netinet/in.h>
#endif

#include <string.h>
#include "PspPacket.h"
#include "ClientInfo.h"

class ServerSocket {
private:
	int sock;
	int port;
	int proto;
	//timeval timeout;
	//fd_set readfds;
#ifdef _WIN32
	typedef int socklen_t;
	static bool init;
#endif

public:
	enum socket_type {TCP_SOCKET, UDP_SOCKET};

	ServerSocket(int port, socket_type proto);
#ifdef _WIN32
	bool WSAStart();
	void WSAClean();
#endif
	bool bindSocket();
	//bool readAvailable();
	bool listenSocket(int max);
	int receive(PspPacket *packet, ClientInfo *info);
	int send(PspPacket *packet, ClientInfo *info);
	void closeSocket();
	virtual ~ServerSocket();
};

#endif /*SERVERSOCKET_H_*/
