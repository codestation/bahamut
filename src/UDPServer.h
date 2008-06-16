#ifndef UDPSERVER_H_
#define UDPSERVER_H_

#include <pthread.h>
#include "PspPacket.h"
#include "DeviceContainer.h"
#include "ServerSocket.h"

class UDPServer {
private:
	pthread_t th;
	int port;
	static bool loop_flag;
	static void *run(void *);
	int receive(PspPacket *packet);
	int send(PspPacket *packet);
public:
	UDPServer(int port);
	void start();
	void stop();
	virtual ~UDPServer();
};

#endif /*UDPSERVER_H_*/
