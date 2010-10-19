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

#include "UDPServer.h"
#include "ClientInfo.h"
#include "Logger.h"

bool UDPServer::loop_flag = true;
u_int UDPServer::server_id = 0;
bool UDPServer::order = true;
ServerSocket *UDPServer::sock = 0;

UDPServer::UDPServer(int port, bool packet_ordering) {
	this->port = port;
	order = packet_ordering;
}


int UDPServer::run() {
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
	INFO("== Server: Initializing UDP server, ID: %X\n", server_id);
	sock = new ServerSocket(port);
	if(sock->bindSocket(AbstractSocket::UDP_SOCKET)) {
		Packet *packet = new Packet();
		List *client = new List(compareFunc, deleteFunc);
		ClientInfo info;
		int size;
		while(loop_flag) {
			if((size = sock->receiveData(packet->getData(), packet->getMaxPacketSize(), (sockaddr *)info.getSocketInfo(), info.getSocketSize())) == -1) {
			//if((size = sock->receiveData(packet, &info)) == -1) {
				if(!sock->readAgain()) {
					ERR("== Server: error occurred while receiving packet\n");
					ERR("== Server: %s (%i)\n", sock->getLastErrorMessage(), sock->getLastError());
				}
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
					INFO("== Server: Registered new client from %s:%i\n", dev->getIPstr(), dev->getPort());
					INFO("== Server: Total clients: %i\n", client->count());
				} else {
					if(packet->getID() == 0) {
						INFO("== Server: Unregistering %s:%i\n", dev->getIPstr(), dev->getPort());
						if(client->remove(dev))
							INFO("== Server: Clients left: %i\n", client->count());
						continue;
					} else {
						if(dev->getID() != packet->getID()) {
							INFO("== Server: The client %s:%i's ID has changed (OLD: %X, NEW: %X), updating...\n", dev->getIPstr(), dev->getPort(), dev->getID(), packet->getID());
							dev->setID(packet->getID());
							dev->setrCounter(0);
							dev->setsCounter(0);
							dev->clearDevices();
						} else {
							if(order && packet->getCounter() <= dev->getrCounter()) {
								DEBUG("=== Server: Packet arrives at wrong order (ID = %X, Expected > %i, Received = %i). Discarding..\n", dev->getID(), dev->getrCounter(), packet->getCounter());
								total_droped++;
								continue;
							}
						}
					}
				}
				EthPacket eth_packet(packet->getPayload());
				//eth_packet.hexdump();
				if(dev->addDevice(eth_packet.getSrcMAC(),0)) {
					INFO("== Server: received MAC: %s from %s:%i\n", eth_packet.getSrcMACstr(), info.getIPstr(), info.getPort());
				}
				if(order)
					dev->setrCounter(packet->getCounter());
				client->rewind();
				while(client->hasNext()) {
					ClientInfo *cl = (ClientInfo *)client->next();
					if(dev != cl) {
						if(eth_packet.isBroadcast() || cl->findDevice((u_char *)eth_packet.getDstMAC())) {
							packet->setID(server_id);
							if(order) {
								packet->setCounter(cl->getsCounter());
								cl->setsCounter(cl->getsCounter() + 1);
							}
							total_sent++;
							total_size_sent += size;
							if(sock->sendData( packet->getData(), packet->getSize(), (sockaddr *)cl->getSocketInfo(), cl->getSocketSize()) == -1) {
								ERR("== Server: error occurred while sending packet\n");
							}
						}
					}
				}
			} else {
				ERR("== Server: Unknown packet. Discarding...\n");
			}
		}
		delete packet;
		delete client;
		sock->closeSocket();
		INFO("\n*** == Server: server finished. Statistics of use:\n");
		INFO("*** == Server: Total packets received: %i\n", total_received);
		INFO("*** == Server: Total packets sent: %i\n", total_sent);
		INFO("*** == Server: Total packets dropped: %i\n", total_droped);
		INFO("*** == Server: Total bytes received: %i\n", total_size_received);
		INFO("*** == Server: Total bytes sent: %i\n", total_size_sent);
	} else {
		ERR("Error while binding the socket\n");
	}
	delete sock;
	return 0;
}

void UDPServer::stop() {
	loop_flag = false;
	if(sock)
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
