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

#include "TCPServer.h"

TCPServer::TCPServer(int port, int max_clients) {
	accept = true;
	ssock = new ServerSocket(port);
	this->max_clients = max_clients;
}

int TCPServer::run() {
	if(ssock->bindSocket(AbstractSocket::TCP_SOCKET))
		if(ssock->listenConnection(max_clients)) {
			while(accept) {
				if((s = ssock->acceptConnection())) {
					INFO("Accepted connection from %s:%i\n", s->getIpAddress(), s->getPort());
					TCPThread *thd = new TCPThread(s);
					thd->start();
					thd->detach();
				} else {
					if(!ssock->readAgain())
						ERR("Error while accepting incoming connection (%s)\n", ssock->getLastErrorMessage());
				}
			}
			INFO("Waiting for the client threads to finish\n");
			ssleep(2);
		} else
			ERR("TCPServer: Cannot listen to socket (%s)\n", ssock->getLastErrorMessage());
	else
		ERR("TCPServer: Cant bind to port\n");
	ssock->closeSocket();
	return 0;
}

void TCPServer::ssleep(int seconds) {
#ifdef _WIN32
	Sleep(1000 * seconds);
#else
	sleep(seconds);
#endif
}

void TCPServer::stop() {
	accept = false;
	TCPThread::stopAll();
}

TCPServer::~TCPServer() {
	ssock->closeSocket();
	delete ssock;
}
