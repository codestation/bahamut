/*
 * SpeedThread.h
 *
 *  Created on: 28/01/2010
 *      Author: code
 */

#ifndef SPEEDTHREAD_H_
#define SPEEDTHREAD_H_

#ifdef _WIN32
#define WINVER 0x0501 //Windows XP
#include <windows.h>
#else
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#endif
//#include <time.h>
//#include <stdio.h>
#include "Logger.h"
#include "Thread.h"

class SpeedThread: public Thread {
private:
	bool flag;
	long last_time;
	unsigned long total_size;
public:
	SpeedThread();
	int run();
	void stop();
	inline void setTime(long time) { last_time = time; }
	inline void addSize(long size) { total_size += size; }
	static long getTick();
	virtual ~SpeedThread();
};

#endif /* SPEEDTHREAD_H_ */
