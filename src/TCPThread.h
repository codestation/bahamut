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
#include "HTTPParser.h"

#define WWWROOT "/home/code/public"

class TCPThread: public Thread {
private:
	Socket *sock;
	static bool stop_all;
	void sendHTTPHeader(int code, const char *message, long int content_size);
	int sendFile(const char *uri);
public:
	TCPThread(Socket *s);
	int run();
	static void stopAll() { stop_all = true; }
	virtual ~TCPThread();
};

#endif /* TCPTHREAD_H_ */
