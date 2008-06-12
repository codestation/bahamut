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

class DeviceInfo {
private:
	u_char mac[6];
	int count;
public:
	DeviceInfo(const u_char *psp_mac);
	const u_char *getMAC();
	const char *getMACstr();
	void setMAC(const u_char *mac);
	int getPacketCounter();
	bool compareMAC(const u_char *mac);
	void setPacketCounter(int newcount);
	virtual ~DeviceInfo();
};

#endif /*DEVICEINFO_H_*/
