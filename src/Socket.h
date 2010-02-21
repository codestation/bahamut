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
#define WINVER 0x0501 //Windows XP
#include <windows.h>
#include <winsock2.h>
#else
#include <unistd.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#endif
#include <string.h>
#include "Packet.h"

#define PACKET_HEADER 10

class Socket
{
private:
	int port;
	int sock;
	char host[16];
	int proto;
	sockaddr_in client;
#ifdef _WIN32
	static bool init;
	LPVOID lpMsgBuf;
#endif

public:
	enum socket_type {TCP_SOCKET, UDP_SOCKET};

	Socket(const char *addr, int port, socket_type proto);
	Socket(int s, sockaddr_in *);
#ifdef _WIN32
	bool WSAStart();
	void WSAClean();
#endif
	bool connectSocket();
	inline int getDescriptor() { return sock; }
	ssize_t readSocket(char *, size_t size);
	ssize_t readSocket(Packet *packet);
	ssize_t writeSocket(const char *, size_t size);
	ssize_t writeSocket(Packet *packet);
	inline const char *getIpAddress() { return inet_ntoa(client.sin_addr); };
	inline int getPort() { return client.sin_port; }
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

	void ssleep(int seconds) {
	#ifdef _WIN32
		Sleep(1000 * seconds);
	#else
		sleep(seconds);
	#endif
	}

	void shutdownSocket() { shutdown(sock, SHUT_RDWR); }
	void closeSocket();
	virtual ~Socket();
};

#endif /*SOCKET_H_*/
