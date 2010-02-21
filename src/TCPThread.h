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

#ifndef TCPTHREAD_H_
#define TCPTHREAD_H_

#include "Thread.h"
#include "Socket.h"
#include "HTTPParser.h"
#include "HTTPResponse.h"

class TCPThread: public Thread {
private:
	Socket *sock;
	HTTPResponse response;
	static bool stop_all;

public:
	TCPThread(Socket *s);
	int run();
	static void stopAll() { stop_all = true; }
	virtual ~TCPThread();
};

#endif /* TCPTHREAD_H_ */
