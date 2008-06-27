/*
 *  Project Bahamut: full ad-hoc tunneling software to be used by the
 *  Playstation Portable (PSP) to emulate online features.
 *
 *  Copyright (C) 2008  Project Bahamut team
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
#include <netdb.h>
#include <unistd.h>
#include <netinet/in.h>
#endif

#include <string.h>
#include "Socket.h"
#include "ClientInfo.h"

class ServerSocket {
private:
	int sock;
	int port;
	int proto;

#ifdef _WIN32
	typedef int socklen_t;
#endif

public:
	ServerSocket(int port, const char *proto);
	bool bindSocket();
	bool listenSocket(int max);
	int receive(PspPacket *packet, ClientInfo *info);
	int send(PspPacket *packet, ClientInfo *info);
	virtual ~ServerSocket();
};

#endif /*SERVERSOCKET_H_*/
