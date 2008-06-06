#ifndef SERVER_H_
#define SERVER_H_

#include <pthread.h>

class Server
{
private:
	pthread_t th;
protected:
	static void *run(void *);
public:
	Server();
	virtual ~Server();
	void start();
};

#endif /*SERVER_H_*/
