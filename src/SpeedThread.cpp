/*
 *  Project Bahamut: full ad-hoc tunneling software to be used by the
 *  Playstation Portable (PSP) to emulate online features.
 *
 *  Copyright (C) 2008-2010  Codestation
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
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
