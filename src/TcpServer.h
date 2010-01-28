/*
 * TcpServer.h
 *
 *  Created on: 27/01/2010
 *      Author: code
 */

#ifndef TCPSERVER_H_
#define TCPSERVER_H_

#include "Thread.h"
#include "Socket.h"

class TcpServer: public Thread {
private:
	Socket *sock;
public:
	TcpServer(void *arg);
	int send(void *data);
	int receive(void *data);
	int run();
	void stop();
	virtual ~TcpServer();
};

#endif /* TCPSERVER_H_ */
