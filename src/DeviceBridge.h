/*
 *  Project Bahamut: full ad-hoc tunneling software to be used by the
 *  Playstation Portable (PSP) to emulate online features.
 *
 *  Copyright (C) 2008  Codestation
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
#define WINVER 0x0501 //Windows XP
#include <windows.h>
#include <process.h>
#else
#include <pthread.h>
#include <sys/times.h>
#endif
#include <time.h>
#include "Thread.h"
#include "Interface.h"
#include "Socket.h"
#include "DeviceInfo.h"
#include "EthPacket.h"
#include "SpeedThread.h"
#include "Logger.h"


class DeviceBridge: public Thread {
public:
	typedef int (*CAPTURE_FUNC) (const u_char *, u_int);
	typedef int (*INJECT_FUNC) (Packet *);

private:
	static void capture_callback(u_char* user, const struct pcap_pkthdr* packet_header, const u_char* packet_data);
	CAPTURE_FUNC cap;
	INJECT_FUNC inj;
	bool loop;
	Packet *cap_packet;
	u_int client_counter;
	u_int server_counter;
	u_int client_id;
	u_int server_id;
	bool order;
	bool buffer;
	bool unregister;
	Socket *sock;
	Interface *eth;

	List *local_mac;
	List *remote_mac;
	SpeedThread *speed;

private:
	static int compareFunc(void *, void *);
	static void deleteFunc(void *);
	void capture(const struct pcap_pkthdr* packet_header, const u_char* packet_data);
public:
	int run();
	DeviceBridge(bool packet_ordering = true, bool packet_buffering = false);
	bool makeBridge(Interface *eth, Socket *sock);
	bool makeBridge(const char *dev, const char *host, int port);
	void removeBridge();
	void registerCaptureCallback(CAPTURE_FUNC func);
	void registerInjectCallback(INJECT_FUNC func);
	const char *getLastError();
	virtual ~DeviceBridge();
};

#endif /*DEVICEBRIDGE_H_*/
