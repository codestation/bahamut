/*
 * TCPThread.h
 *
 *  Created on: 29/01/2010
 *      Author: code
 */

#ifndef TCPTHREAD_H_
#define TCPTHREAD_H_

#include "Thread.h"
#include "Socket.h"
#include "StreamSocket.h"

class TCPThread: public Thread {
private:
	Socket *sock;
	static bool stop_all;
public:
	TCPThread(Socket *s);
	int run();
	static void stopAll() { stop_all = true; }
	virtual ~TCPThread();
};

#endif /* TCPTHREAD_H_ */
