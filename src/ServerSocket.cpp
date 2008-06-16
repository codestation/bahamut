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
*	TCP server
*Special Notes:
*	TODO: incomplete class
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

#include "ServerSocket.h"

ServerSocket::ServerSocket(int port, const char *proto) {
	sock = 0;
	if(strcmp(proto,"tcp") == 0) {
		this->proto = IPPROTO_TCP;
	} else if (strcmp(proto,"udp") == 0) {
		this->proto = IPPROTO_UDP;
	} else {
		this->proto = -1;
	}
	this->port = port;
}

bool ServerSocket::bindSocket() {
	if((sock = socket(PF_INET, proto == IPPROTO_TCP ? SOCK_STREAM : SOCK_DGRAM, proto)) >= 0) {
		server.sin_family = AF_INET;
		server.sin_port = htons(port);
		server.sin_addr.s_addr = htonl (INADDR_ANY);
		return bind( sock, (struct sockaddr *)&server, sizeof(server)) != -1;
	} else {
		return false;
	}
}

bool ServerSocket::listenSocket(int max) {
	return listen( sock, max) != -1;
}

Socket *ServerSocket::acceptSocket() {
	socklen_t addrlen = sizeof(server);
	int s = accept( sock, (struct sockaddr *)&server, &addrlen);
	return s != 0 ? new Socket(s) : 0;
}

int ServerSocket::receive(PspPacket *packet) {
	socklen_t addrlen = sizeof(server);
	int res = recvfrom( sock, packet->getPacketData(), packet->getPacketSize(), 0, (struct sockaddr *)&server, &addrlen);
	packet->setIP(server.sin_addr.s_addr);
	packet->setPort(server.sin_port);
	return res;
}

int ServerSocket::send(PspPacket *packet) {
	server.sin_port = packet->getPort();
	server.sin_addr.s_addr = packet->getIP();
	return sendto( sock, packet->getPacketData(), packet->getPacketSize(), 0, (struct sockaddr *)&server,sizeof(server));
}

ServerSocket::~ServerSocket() {

}
