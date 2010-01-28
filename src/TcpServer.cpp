/*
 * TcpServer.cpp
 *
 *  Created on: 27/01/2010
 *      Author: code
 */

#include "TcpServer.h"

TcpServer::TcpServer(void *arg) {
	sock = (Socket *)arg;
}

int TcpServer::run() {
	sock->closeSocket();
	return 0;
}

TcpServer::~TcpServer() {
	// TODO Auto-generated destructor stub
}
