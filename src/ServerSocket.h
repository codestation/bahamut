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
#include <errno.h>
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
	LPVOID lpMsgBuf;
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
	inline const char *getLastErrorMessage() {
#ifdef _WIN32
		if(lpMsgBuf)
			LocalFree(lpMsgBuf);
		FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER |
				FORMAT_MESSAGE_FROM_SYSTEM |
				FORMAT_MESSAGE_IGNORE_INSERTS, NULL, WSAGetLastError(),
				MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
				(LPTSTR) &lpMsgBuf,
				0, NULL);
		return (const char *)lpMsgBuf;
#else
		return strerror(errno);
#endif
	}

	inline bool readAgain() {
#ifdef _WIN32
		return WSAGetLastError() == WSAEWOULDBLOCK || WSAETIMEDOUT;
#else
		return errno == EAGAIN;
#endif
	}

	inline int getLastError() {
#ifdef _WIN32
		return WSAGetLastError();
#else
		return errno;
#endif
	}

	void closeSocket();
	virtual ~ServerSocket();
};

#endif /*SERVERSOCKET_H_*/
