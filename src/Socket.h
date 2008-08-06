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
 *     TODO: none yet
 */

#ifndef SOCKET_H_
#define SOCKET_H_

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

#define PACKET_HEADER 10

class Socket
{
private:
	int port;
	int sock;
	int socket_flag;
	char *host;
	int proto;
	sockaddr_in client;
#ifdef _WIN32
	static bool init;
#endif

public:
	Socket(const char *addr, int port, const char *proto);
	Socket(int s);
#ifdef _WIN32
	bool WSAStart();
	void WSAClean();
#endif
	bool connectSocket();
	ssize_t readSocket(char *, size_t size);
	ssize_t readSocket(PspPacket *packet);
	ssize_t writeSocket(const char *, size_t size);
	ssize_t writeSocket(PspPacket *packet);
	void closeSocket();
	virtual ~Socket();
};

#endif /*SOCKET_H_*/
