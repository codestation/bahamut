/*
 * ServerThread.cpp
 *
 *  Created on: 28/01/2010
 *      Author: code
 */

#include "Thread.h"

Thread::Thread() {
}

int Thread::run() {
	return 0;
}

#ifdef _WIN32
unsigned __stdcall Thread::_run(void *arg) {
	return ((Thread *)arg)->run();
}
#else
void *Thread::_run(void *arg) {
	return (void *)((Thread *)arg)->run();
}
#endif

void Thread::start() {
#ifdef _WIN32
	th = _beginthreadex(NULL, 4096, &_run, this, 0, NULL);
#else
	pthread_create(&th, NULL, &_run, this);
#endif
}

int Thread::wait() {
#ifdef _WIN32
	return WaitForSingleObject((void *)th, INFINITE);
#else
	return pthread_join(th, NULL);
#endif
}

Thread::~Thread() {
	// TODO Auto-generated destructor stub
}
