/*
 * ServerThread.h
 *
 *  Created on: 28/01/2010
 *      Author: code
 */

#ifndef SERVERTHREAD_H_
#define SERVERTHREAD_H_

#ifdef _WIN32
#define WINVER 0x0501 //Windows XP
#include <windows.h>
#include <process.h>
#else
#include <pthread.h>
#endif

class Thread {
private:
#ifdef _WIN32
	unsigned int th;
#else
	pthread_t th;
#endif

#ifdef _WIN32
	static unsigned __stdcall _run(void *);
#else
	static void *_run(void *);
#endif
protected:
	virtual int run();
public:
	Thread();
	void start();
	int detach();
	int wait();
	virtual ~Thread();
};

#endif /* SERVERTHREAD_H_ */
