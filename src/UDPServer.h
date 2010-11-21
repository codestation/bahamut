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

#ifndef UDPSERVER_H_
#define UDPSERVER_H_

#ifdef _WIN32
#define WINVER 0x0501 //Windows XP
#include <windows.h>
#include <process.h>
#else
#include <pthread.h>
#endif
#include <time.h>
#include <errno.h>
#include "core/Packet.h"
#include "core/Thread.h"
#include "core/EthPacket.h"
#include "core/ServerSocket.h"
#include "core/List.h"

class UDPServer: public Thread {

	int port;
	static u_int server_id;
	static bool loop_flag;
	static bool order;
	static ServerSocket *sock;
	int receive(Packet *packet);
	int send(Packet *packet);
	static int compareFunc(void *, void *);
	static void deleteFunc(void *);
public:
	UDPServer(int port, bool packet_ordering = true);
	void stop();
	int run();
	virtual ~UDPServer();
};

#endif /*UDPSERVER_H_*/
