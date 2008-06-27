/*
 *  Project Bahamut: full ad-hoc tunneling software to be used by the
 *  Playstation Portable (PSP) to emulate online features.
 *
 *  Copyright (C) 2008  Project Bahamut team
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

/*
 * File Description:
 *     Sends the capture packets to a main server, then receive the
 *     packets of the other clients and injects on the interface
 *
 * Special Notes:
 *     TODO: get rid of all those static things :/
 */

#ifndef DEVICEBRIDGE_H_
#define DEVICEBRIDGE_H_

#ifdef _WIN32
#include <windows.h>
#include <process.h>
#else
#include <pthread.h>
#endif
#include "Interface.h"
#include "Socket.h"


class DeviceBridge
{
public:
	typedef int (*CAPTURE_FUNC) (const u_char *, u_int);
	typedef int (*INJECT_FUNC) (PspPacket *);
private:
	static void capture_callback(u_char* user, const struct pcap_pkthdr* packet_header, const u_char* packet_data);
	static CAPTURE_FUNC cap;
	static INJECT_FUNC inj;
	static bool loop;
	static PspPacket *cap_packet;
	static u_int client_counter;
	static u_int server_counter;
	static u_int client_id;
	static u_int server_id;
	static Socket *sock;
	static Interface *eth;
#ifdef _WIN32
	unsigned int th;
#else
	pthread_t th;
#endif

public:
	DeviceBridge();
	bool makeBridge(Interface *eth, Socket *sock);
	bool makeBridge(const char *dev, const char *host, int port);
	void removeBridge();
	void registerCaptureCallback(CAPTURE_FUNC func);
	void registerInjectCallback(INJECT_FUNC func);
	const char *getLastError();
#ifdef _WIN32
	static void inject_thread(void *arg);
#else
	static void *inject_thread(void *arg);
#endif
	virtual ~DeviceBridge();
};

#endif /*DEVICEBRIDGE_H_*/
