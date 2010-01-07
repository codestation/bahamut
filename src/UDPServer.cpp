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
 *     UDP Server engine
 * Special Notes:
 *     TODO: remove debug messages, more error checking
 */

#include "UDPServer.h"

bool UDPServer::loop_flag = true;
u_int UDPServer::server_id = 0;
bool UDPServer::order = true;
ServerSocket *UDPServer::sock = 0;

UDPServer::UDPServer(int port, bool packet_ordering) {
	this->port = port;
	order = packet_ordering;
}

#ifdef _WIN32
void UDPServer::run(void *arg) {
#else
void *UDPServer::run(void *arg) {
#endif
	u_int total_received = 0;
	u_int total_sent = 0;
	u_int total_size_sent = 0;
	u_int total_size_received = 0;
	u_int total_droped = 0;
	srand(time(0));
#ifdef _WIN32
	server_id = (rand() << 16) | rand();
#else
	server_id = rand();
#endif
	if(server_id == 0)
		server_id++;
	printf("== Server: Initializing UDP server, ID: %X\n", server_id);
	sock = new ServerSocket(*(int *)arg, "udp");
#ifdef _WIN32
	if(sock->WSAStart()) {
#endif
		if(sock->bindSocket()) {
			PspPacket *packet = new PspPacket();
			List *client = new List(compareFunc, deleteFunc);
			ClientInfo info;
			int size;
			while(loop_flag) {
				if((size = sock->receive(packet, &info)) == -1) {
					printf("== Server: error occurred while receiving packet\n");
					continue;
				}
				total_size_received += size;
				total_received++;
				if(packet->checkHeader()) {
					ClientInfo *dev = (ClientInfo *)client->get(&info);
					if(dev == NULL) {
						dev = new ClientInfo(&info);
						client->add(dev);
						dev->setID(packet->getID());
						printf("== Server: Registered new client from %s:%i\n", dev->getIPstr(), dev->getPort());
						printf("== Server: Total clients: %i\n", client->count());
					} else {
						if(packet->getID() == 0) {
							printf("== Server: Unregistering %s:%i\n", dev->getIPstr(), dev->getPort());
							if(client->remove(dev))
								printf("== Server: Clients left: %i\n", client->count());
							continue;
						} else {
							if(dev->getID() != packet->getID()) {
								printf("== Server: The client %s:%i's ID has changed (OLD: %X, NEW: %X), updating...\n", dev->getIPstr(), dev->getPort(), dev->getID(), packet->getID());
								dev->setID(packet->getID());
								dev->setrCounter(0);
								dev->setsCounter(0);
								dev->clearDevices();
							} else {
								if(order && packet->getPacketCounter() <= dev->getrCounter()) {
									//printf("=== Server: Packet arrives at wrong order (ID = %X, Expected > %i, Received = %i). Discarding..\n", dev->getID(), dev->getrCounter(), packet->getPacketCounter());
									total_droped++;
									continue;
								}
							}
						}
					}
					if(dev->addDevice(packet->getSrcMAC(),0)) {
						printf("== Server: received MAC: %s from %s:%i\n", packet->getSrcMACstr(), info.getIPstr(), info.getPort());
					}
					if(order)
						dev->setrCounter(packet->getPacketCounter());
					client->rewind();
					while(client->hasNext()) {
						ClientInfo *cl = (ClientInfo *)client->next();
						if(dev != cl) {
							if(packet->isBroadcast() || cl->findDevice((u_char *)packet->getDstMAC())) {
								packet->setID(server_id);
								if(order) {
									packet->setPacketCounter(cl->getsCounter());
									cl->setsCounter(cl->getsCounter() + 1);
								}
								total_sent++;
								total_size_sent += size;
								if(sock->send( packet, cl) == -1) {
									printf("== Server: error occurred while sending packet\n");
								}
							}
						}
					}
				} else {
					printf("== Server: Unknown packet. Discarding...\n");
				}
			}
			delete packet;
			delete client;
			sock->closeSocket();
			printf("*** == Server: server finished. Statistics of use:\n");
			printf("*** == Server: Total packets received: %i\n", total_received);
			printf("*** == Server: Total packets sent: %i\n", total_sent);
			printf("*** == Server: Total packets dropped: %i\n", total_droped);
			printf("*** == Server: Total bytes received: %i\n", total_size_received);
			printf("*** == Server: Total bytes sent: %i\n", total_size_sent);
		} else {
			printf("Error while binding the socket\n");
		}
		delete sock;
#ifdef _WIN32
	}
#else
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
	sock->closeSocket();
}

int UDPServer::compareFunc(void *obj, void *item) {
	return ((ClientInfo *)obj)->compareTo((ClientInfo *)item);
}

void UDPServer::deleteFunc(void *obj) {
	delete (ClientInfo *)obj;
}

UDPServer::~UDPServer() {

}
