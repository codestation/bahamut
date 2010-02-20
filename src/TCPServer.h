/*
 * TcpServer.h
 *
 *  Created on: 27/01/2010
 *      Author: code
 */

#ifndef TCPSERVER_H_
#define TCPSERVER_H_

#include "Thread.h"
#include "ServerSocket.h"
#include "TCPThread.h"
#include "Socket.h"
#include "Logger.h"

class TCPServer: public Thread {
private:
	ServerSocket *ssock;
	Socket *s;
	int port;
	bool accept;
	int max_clients;

	void ssleep(int seconds);

public:
	TCPServer(int port, int max_clients = 3);
	int send(void *data);
	int receive(void *data);
	int run();
	void stop();
	virtual ~TCPServer();
};

#endif /* TCPSERVER_H_ */
