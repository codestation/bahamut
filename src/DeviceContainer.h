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
*	A simple vector to help organize the device list
*Special Notes:
*	TODO: Only holds 4 Devices MAX, no index bounds checking implemented yet
* 	It uses a static vector to dont deal with dynamic memory alloc (for now)
* 	(I know, its ugly, but its faster and simple)
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


#ifndef DEVICECONTAINER_H_
#define DEVICECONTAINER_H_

#include "DeviceInfo.h"

class DeviceContainer
{
private:
	DeviceInfo *dev[4];
public:
	DeviceContainer();
	bool addDevice(DeviceInfo *psp);
	bool removeDevice(const u_char *mac);
	DeviceInfo *getDevice(const u_char *mac);
	DeviceInfo *getDeviceAtPos(int pos);
	int deviceCount();
	virtual ~DeviceContainer();
};

#endif /*DEVICECONTAINER_H_*/
