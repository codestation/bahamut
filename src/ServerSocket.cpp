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

ServerSocket::ServerSocket()
{
}

ServerSocket::~ServerSocket()
{
}

bool ServerSocket::bindSocket() {
	client.sin_family = AF_INET;
	client.sin_port = htons(port);
	client.sin_addr.s_addr = htonl (INADDR_ANY);
	return bind( sock, (struct sockaddr *)&client, sizeof(client)) != -1;
}

bool ServerSocket::listenSocket(int max) {
	return listen( sock, max) != -1;
}

Socket *ServerSocket::acceptSocket() {
	socklen_t addrlen = sizeof(client);
	int s = accept( sock, (struct sockaddr *)&client, &addrlen);
	return s != 0 ? new Socket(s) : 0;
}
