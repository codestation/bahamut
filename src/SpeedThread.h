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
#include "core/Logger.h"
#include "core/Thread.h"

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
