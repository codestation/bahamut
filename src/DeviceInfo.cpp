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

#include "DeviceInfo.h"

DeviceInfo::DeviceInfo(const u_char *psp_mac) {
	count = 0;
	memcpy(mac, psp_mac, 6);
}
const u_char *DeviceInfo::getMAC() {
	return mac;
}

const char *DeviceInfo::getMACstr() {
	char *mac_str = new char[18];
	sprintf(mac_str,"%.2x:%.2x:%.2x:%.2x:%.2x:%.2x", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
	return mac_str;
}

void DeviceInfo::setMAC(const u_char *psp_mac) {
	memcpy(mac, psp_mac, 6);
}
int DeviceInfo::getPacketCounter() {
	return count;
}
void DeviceInfo::setPacketCounter(int newcount) {
	count = newcount;
}
bool DeviceInfo::compareMAC(const u_char *mac) {
	return memcmp(this->mac, mac, 6) == 0;	
}
DeviceInfo::~DeviceInfo() {
	
}
