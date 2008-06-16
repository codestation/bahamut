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
	port = 0;
	ip_addr = 0;
	memcpy(mac, psp_mac, 6);
}

DeviceInfo::DeviceInfo(in_addr_t ip, int port) {
	count = 0;
	this->port = port;
	ip_addr = ip;
	memset(mac, 6, 0);
}

inline const u_char *DeviceInfo::getMAC() {
	return mac;
}

char *DeviceInfo::getMACstr() {
	sprintf(mac_str,"%.2x:%.2x:%.2x:%.2x:%.2x:%.2x", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
	return mac_str;
}

char *DeviceInfo::getIPstr() {
	in_addr tmp;
	tmp.s_addr = ip_addr;
	return inet_ntoa(tmp);
}

void DeviceInfo::setMAC(const u_char *psp_mac) {
	memcpy(mac, psp_mac, 6);
}
u_int DeviceInfo::getPacketCounter() {
	return count;
}
void DeviceInfo::setRandomID() {
	id = rand();
}
void DeviceInfo::setID(u_int newid) {
	id = newid;
}
u_int DeviceInfo::getID() {
	return id;
}

void DeviceInfo::setIP(in_addr_t ip) {
	ip_addr = ip;
}
void DeviceInfo::setPort(int port) {
	this->port = port;
}
in_addr_t DeviceInfo::getIP() {
	return ip_addr;
}
int DeviceInfo::getPort() {
	return port;
}
void DeviceInfo::setPacketCounter(int newcount) {
	count = newcount;
}
bool DeviceInfo::compareMAC(const u_char *mac) {
	return memcmp(this->mac, mac, 6) == 0;	
}
DeviceInfo::~DeviceInfo() {
	
}
