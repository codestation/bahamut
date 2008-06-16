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

#include "DeviceContainer.h"

DeviceContainer::DeviceContainer() {
	dev[0] = 0;
	dev[1] = 0;
	dev[2] = 0;
	dev[3] = 0;
}
bool DeviceContainer::addDevice(DeviceInfo *psp) {
	int i = 0;
	while(i < 4) {
		if(dev[i] == NULL) {
			dev[i] = psp;
			return true;
		}
		i++;
	}
	return false;
}
bool DeviceContainer::removeDevice(const u_char *mac) {
	int i = 0;
	while(i < 4) {
		if(dev[i] != NULL) {
			if(dev[i]->compareMAC(mac))
				delete dev[i];
				dev[i] = NULL;
				return true;
		}
		i++;
	}
	return false;
}
DeviceInfo *DeviceContainer::getDevice(const u_char *mac) {
	int i = 0;
	while(i < 4) {
		if(dev[i] != NULL) {
			if(dev[i]->compareMAC(mac))
				return dev[i];
		}
		i++;
	}
	return NULL;
}
DeviceInfo *DeviceContainer::getDevice(in_addr_t ip, int port) {
	int i = 0;
	while(i < 4) {
		if(dev[i] != NULL) {
			if(dev[i]->getIP() == ip && dev[i]->getPort() == port)
				return dev[i];
		}
		i++;
	}
	return NULL;
}
int DeviceContainer::deviceCount() {
	int count = 0;
	for(int i = 0;i < 4; i++)
		if(dev[i] != NULL)
			count++;
	return count;
}

DeviceInfo *DeviceContainer::getDeviceAtPos(int pos) {
	return pos >=0 && pos < 4 ? dev[pos] : NULL;
}

DeviceContainer::~DeviceContainer() {
	for(int i = 0;i < 4; i++)
		if(dev[i] != NULL)
			delete dev[i];
}
