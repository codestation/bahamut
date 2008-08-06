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
 *     Holds some client info (ip addres:port and client ID)
 * Special Notes:
 *     TODO: none yet
 */

#ifndef CLIENTINFO_H_
#define CLIENTINFO_H_

#ifdef _WIN32
#include <windows.h>
#include <winsock2.h>
#else
#include <netinet/in.h>
#include <arpa/inet.h>
#endif

#include <string.h>
#include "DeviceInfo.h"
#include "List.h"

class ClientInfo {
private:
	sockaddr_in client;
	u_int id;
	u_int r_counter;
	u_int s_counter;
	List *psp;
	static int compare_func(void *, void *);
	static void delete_func(void *);
public:
	ClientInfo();
	ClientInfo(ClientInfo *);
	ClientInfo(in_addr ip, int port);
	void reset();
	sockaddr_in *getSocketInfo();
	int getSocketSize();
	in_addr getIP();
	int getPort();
	void setID(u_int id);
	u_int getID();
	char *getIPstr();
	void setrCounter(u_int c);
	u_int getrCounter();
	void setsCounter(u_int c);
	u_int getsCounter();
	int compareTo(ClientInfo *);
	bool addDevice(const u_char *mac);
	bool findDevice(const u_char *mac);
	void clearDevices();
	virtual ~ClientInfo();
};

#endif /*CLIENTINFO_H_*/
