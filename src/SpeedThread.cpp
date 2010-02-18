/*
 * SpeedThread.cpp
 *
 *  Created on: 28/01/2010
 *      Author: code
 */

#include "SpeedThread.h"

SpeedThread::SpeedThread() {
	flag = true;
	last_time = 0;
	total_size = 0;
}

int SpeedThread::run() {
	u_int bytes = 0;
	while(flag) {
		int actual = getTick();
		int last = last_time;
		int msecs = actual - last;
		if(msecs < 0)
			msecs = -msecs;
		bytes = (total_size - bytes);
		INFO("Download: %i bytes/s (%i kbps), Last: %i secs ago (%i msecs ago)   ", bytes, bytes/128, msecs /1000, msecs);
		bytes = total_size;
		fflush(stdout);
#ifdef _WIN32
		Sleep(1000);
#else
		sleep(1);
#endif
		INFO("\r");
	}
	return 0;
}

void SpeedThread::stop() {
	flag = false;
}

long SpeedThread::getTick() {
#ifdef _WIN32
	return GetTickCount();
#else
	timeval tv;
	gettimeofday(&tv, NULL);
	return (tv.tv_sec * 1000) + (tv.tv_usec / 1000);
#endif
}

SpeedThread::~SpeedThread() {
	// TODO Auto-generated destructor stub
}
