/*
 * TcpServer.cpp
 *
 *  Created on: 27/01/2010
 *      Author: code
 */

#include "TCPServer.h"

TCPServer::TCPServer(int port, int max_clients) {
	accept = true;
	ssock = new ServerSocket(port, ServerSocket::TCP_SOCKET);
	this->max_clients = max_clients;
}

int TCPServer::run() {
	if(ssock->bindSocket())
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
