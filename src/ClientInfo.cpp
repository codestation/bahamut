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

#include "ClientInfo.h"

ClientInfo::ClientInfo() {
	memset(&client, 0, sizeof(client));
	id = 0;
	r_counter = 0;
	s_counter = 0;
	psp = new List(compare_func, delete_func);
}

void ClientInfo::reset() {
	memset(&client, 0, sizeof(client));
	id = 0;
	r_counter = 0;
	s_counter = 0;
}

sockaddr_in *ClientInfo::getSocketInfo() {
	return &client;
}

int ClientInfo::getSocketSize() {
	return sizeof(client);
}

ClientInfo::ClientInfo(ClientInfo *info) {
	memcpy(&client, info->getSocketInfo(), sizeof(client));
	id = info->id;
	r_counter = 0;
	s_counter = 0;
	psp = new List(compare_func, delete_func);
}

void ClientInfo::setID(u_int cid) {
	id = cid;
}
u_int ClientInfo::getID() {
	return id;
}

int ClientInfo::getPort() {
	return client.sin_port;
}

char *ClientInfo::getIPstr() {
	return inet_ntoa(client.sin_addr);
}

void ClientInfo::setrCounter(u_int c) {
	r_counter = c;
}
u_int ClientInfo::getrCounter() {
	return r_counter;
}
void ClientInfo::setsCounter(u_int c) {
	s_counter = c;
}
u_int ClientInfo::getsCounter() {
	return s_counter;
}
int ClientInfo::compareTo(ClientInfo *info) {
	return memcmp(&client, info->getSocketInfo(), sizeof(client));
}

bool ClientInfo::addDevice(const u_char *mac, u_int id) {
	if(!psp->exist((void *)mac)) {
		psp->add(new DeviceInfo(mac, id));
		return true;
	}
	return false;
}

bool ClientInfo::findDevice(const u_char *mac) {
	return psp->exist((void *)mac);
}

int ClientInfo::compare_func(void *obj, void *item) {
	return ((DeviceInfo *)obj)->compareMAC((u_char *)item);
}

void ClientInfo::delete_func(void *obj) {
	delete ((DeviceInfo *)obj);
}

void ClientInfo::clearDevices() {
	psp->clear();
}

ClientInfo::~ClientInfo() {
	delete psp;
}
