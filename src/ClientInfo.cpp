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
 *     Holds some client info (ip addres:port and client ID)
 * Special Notes:
 *     TODO: none yet
 */

#include "ClientInfo.h"

ClientInfo::ClientInfo() {
	memset(&client, 0, sizeof(client));
	id = 0;
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

void ClientInfo::setCounter(u_int c) {
	counter = c;
}
u_int ClientInfo::getCounter() {
	return counter;
}
bool ClientInfo::compareTo(ClientInfo *info) {
	return !memcmp(&client, info->getSocketInfo(), sizeof(client));
}

ClientInfo::~ClientInfo() {

}
