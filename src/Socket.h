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
*	none yet
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


#ifndef SOCKET_H_
#define SOCKET_H_

#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
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
	u_int packet_count;
	sockaddr_in client;

	in_addr_t resolve(const char *url);
public:
	Socket(const char *addr, int port, const char *proto);
	Socket(int s);
	virtual ~Socket();
	bool connectSocket();
	ssize_t readSocket(void *, size_t size);
	ssize_t readSocket(PspPacket *packet);
	ssize_t writeSocket(const void *, size_t size);
	ssize_t writeSocket(PspPacket *packet);
	void closeSocket();
};

#endif /*SOCKET_H_*/
