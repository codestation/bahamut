/*
*Project Name: Bahamut
* 
*Programmers: Codestation, ZackVixACD
* 
*Project Descrption: The project bahamut is a full ad-hoc tunneling
*	software to be used by the Playstation Portable (PSP) to emulate 
*	online features.
*
*File Description:
*	Holds some device info (MAC address, current packet count)
*Special Notes:
*	TODO: add more info: IP address, and some account info.
*
*Copyright Stuff:
*   This program is free software: you can redistribute it and/or modify
*   it under the terms of the GNU General Public License as published by
*   the Free Software Foundation, either version 3 of the License, or
*   (at your option) any later version.
* 
*   This program is distributed in the hope that it will be useful,
*   but WITHOUT ANY WARRANTY; without even the implied warranty of
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*   GNU General Public License for more details.
* 
*  You should have received a copy of the GNU General Public License
*   along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef DEVICEINFO_H_
#define DEVICEINFO_H_

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>

class DeviceInfo {
private:
	u_char mac[6];
	u_int count;
	in_addr_t ip_addr;
	int port;
	u_int id;
	//temporary buffer to hold the MAC string
	char mac_str[18];	
public:
	DeviceInfo(const u_char *psp_mac);
	DeviceInfo(in_addr_t ip, int port);
	const u_char *getMAC();
	char *getMACstr();
	char *getIPstr();
	void setMAC(const u_char *mac);
	void setIP(in_addr_t ip);
	void setPort(int port);
	in_addr_t getIP();
	int getPort();
	u_int getPacketCounter();
	void setRandomID();
	void setID(u_int id);
	u_int getID();
	bool compareMAC(const u_char *mac);
	void setPacketCounter(int newcount);
	virtual ~DeviceInfo();
};

#endif /*DEVICEINFO_H_*/
