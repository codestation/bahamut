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
 *     UDP Server engine
 * Special Notes:
 *     TODO: remove debug messages, more error checking
 */

#include "UDPServer.h"

bool UDPServer::loop_flag = true;
u_int UDPServer::server_id = 0;

UDPServer::UDPServer(int port) {
	this->port = port;
}

#ifdef _WIN32
void UDPServer::run(void *arg) {
#else
void *UDPServer::run(void *arg) {
#endif
	srand(time(0));
	server_id = rand();
	printf("=== Server: Initializing UDP server, ID: %X\n", server_id);
	ServerSocket *sock = new ServerSocket(*(int *)arg, "udp");
	sock->bindSocket();
	PspPacket *packet = new PspPacket();
	List *client = new List(compareFunc, deleteFunc);
	ClientInfo info;
	while(loop_flag) {
		sock->receive(packet, &info);
		if(packet->checkHeader()) {
			ClientInfo *dev = (ClientInfo *)client->get(&info);
			if(dev == NULL) {
				dev = new ClientInfo(&info);
				client->add(dev);
				dev->setID(packet->getID());
				printf("=== Server: Registered new client from %s:%i\n", dev->getIPstr(), dev->getPort());
			} else {
				if(dev->getID() != packet->getID()) {
					printf("=== Server: The client %s:%i's ID has changed (OLD: %X, NEW: %X), updating...\n", dev->getIPstr(), dev->getPort(), dev->getID(), packet->getID());
					dev->setID(packet->getID());
				} else {
					if(packet->getPacketCounter() <= dev->getCounter()) {
						printf("=== Server: Packet arrives at wrong order (Expected > %i, Received = %i). Discarding..\n", dev->getCounter(), packet->getPacketCounter());
						continue;
					}
				}
			}
			dev->setCounter(packet->getPacketCounter());
			client->rewind();
			while(client->hasNext()) {
				ClientInfo *cl = (ClientInfo *)client->next();
				if(dev != cl) {
					packet->setID(server_id);
					sock->send( packet, cl);
				}
			}
		} else {
			printf("=== Server: Unknown packet. Discarding...\n");
		}
	}
	delete packet;
	delete client;
	delete sock;
#ifndef _WIN32
	return 0;
#endif
}

void UDPServer::start() {
#ifdef _WIN32
	th = _beginthread(&run, 4096, &port);
#else
	pthread_create(&th, NULL, &run, &port);
#endif
}

void UDPServer::stop() {
	loop_flag = false;
}

int UDPServer::compareFunc(void *obj, void *item) {
	return ((ClientInfo *)obj)->compareTo((ClientInfo *)item) ? 0 : 1;
}

void UDPServer::deleteFunc(void *obj) {
	delete (ClientInfo *)obj;
}

UDPServer::~UDPServer() {

}
