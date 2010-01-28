
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
 *     Sends the capture packets to a main server, then receive the
 *     packets of the other clients and injects on the interface
 *
 * Special Notes:
 *     TODO: get rid of all those static things :/
 */

#include "DeviceBridge.h"

// FIXME: I know this is very ugly but its only for logging purposes
// I have to remove this crap ASAP and clean the code

Packet *DeviceBridge::cap_packet = 0;
u_int DeviceBridge::client_counter = 0;
u_int DeviceBridge::server_counter = 0;
u_int DeviceBridge::client_id = 0;
u_int DeviceBridge::server_id = 0;
Socket *DeviceBridge::sock = 0;
Interface *DeviceBridge::eth = 0;
List *DeviceBridge::local_mac = 0;
List *DeviceBridge::remote_mac = 0;
bool DeviceBridge::loop = true;
bool DeviceBridge::unregister = false;
bool DeviceBridge::order = true;
bool DeviceBridge::buffer = false;
bool DeviceBridge::server_conn = false;
DeviceBridge::CAPTURE_FUNC DeviceBridge::cap = 0;
DeviceBridge::INJECT_FUNC DeviceBridge::inj = 0;

u_int DeviceBridge::total_droped = 0;
u_int DeviceBridge::total_received = 0;
u_int DeviceBridge::total_sent = 0;
u_int DeviceBridge::total_size_received = 0;
u_int DeviceBridge::total_size_sent = 0;
//u_int DeviceBridge::mac_count = 0;

//u_int DeviceBridge::total_broadcast_received = 0;
//u_int DeviceBridge::total_broadcast_sent = 0;
//u_int DeviceBridge::total_broadcast_size_received = 0;
//u_int DeviceBridge::total_broadcast_size_sent = 0;

long DeviceBridge::last_packet = 0;

DeviceBridge::DeviceBridge(bool packet_ordering, bool packet_buffering) {
	order = packet_ordering;
	buffer = packet_buffering;
	cap_packet = new Packet();
	speed = new SpeedThread();
	last_packet = speed->getTick();
	local_mac = new List(compareFunc, deleteFunc);
	remote_mac = new List(compareFunc, deleteFunc);
	srand(time(0));
#ifdef _WIN32
	client_id = (rand() << 16) | rand();
#else
	client_id = rand();
#endif
	if(client_id == 0)
		client_id++;
	printf("Initializing client, ID: %X\n", client_id);
}

bool DeviceBridge::makeBridge(const char *dev, const char *host, int port) {
	eth = new Interface(dev);
	sock = new Socket(host, port, Socket::UDP_SOCKET);
	bool res = makeBridge(eth, sock);
	delete eth;
	delete sock;
	return res;
}
bool DeviceBridge::makeBridge(Interface *eth, Socket *sock) {
	if(eth->open()) {
#ifdef _WIN32
		if(sock->WSAStart()) {
#endif
			if(sock->connectSocket()) {
				this->start();
				speed->start();
				sprintf(buffer_data, "not ether src %s", eth->getMacAddressStr());
				INFO("Filter: %s\n", buffer_data);
				eth->compileFilter(buffer_data);
				eth->setFilter();
				eth->setdirection();
				// starts loop, doesnt return until close
				eth->captureLoop(capture_callback);
#ifdef _WIN32
				sock->WSAClean();
#endif
				eth->close();
				sock->closeSocket();
			}
#ifdef _WIN32
		}
#endif
	}
	return true;
}

void DeviceBridge::capture_callback(u_char* user, const struct pcap_pkthdr* packet_header, const u_char* packet_data) {
	if(loop) {
		if(packet_header->len <= MAX_PAYLOAD_SIZE) {
			if(cap && cap(packet_data, packet_header->len))
				return;
			EthPacket eth_packet(packet_data);
			cap_packet->setPayload(&eth_packet, packet_header->len);
			cap_packet->setCounter(client_counter++);
			cap_packet->setID(client_id);
			if(!local_mac->exist((void *)eth_packet.getSrcMAC()) && local_mac->count() < 16) {
				local_mac->add(new DeviceInfo(eth_packet.getSrcMAC(), local_mac->count()));
				INFO("\nRegistered new MAC: %s\n", eth_packet.getSrcMACstr());
			}
			if(local_mac->exist((void *)eth_packet.getDstMAC())) {
				INFO("\nPacket destination is local, ignoring\n");
			} else {
				if(sock->writeSocket(cap_packet)) {
					INFO("capture_callback: end of stream reached. Finishing thread...\n");
					eth->breakLoop();
					loop = false;
					return;
				}
			}
		}
	} else {
		if(unregister) {
			cap_packet->setPayload((u_char *)"END",3);
			cap_packet->setID(0);
			INFO("Sending unregister packet to server\n");
			sock->writeSocket(cap_packet);
			sock->closeSocket();
		}
		printf("End of capture_callback. Finishing loop\n");
	}
}

int DeviceBridge::run() {
	Packet *packet = new Packet();
	int size;
	while(loop) {
		if((size = sock->readSocket(packet)) < 0) {
			INFO("inject_thread: end of stream reached. Finishing thread...\n");
			loop = false;
			eth->breakLoop();
			break;
		}
		last_packet = speed->getTick();
		//FIXME: remove this var
		total_size_received += size;
		if(packet->checkHeader()) {
			if(!server_conn) {
				server_id = packet->getID();
				INFO("Registered new server with ID: %X\n", server_id);
				server_conn = true;
			} else {
				if(server_id != packet->getID()) {
					INFO("Server ID changed: %X ==> %X\n", server_id, packet->getID());
					server_id = packet->getID();
				} else {
					if(order && packet->getCounter() <= server_counter) {
						INFO("Packet arrives at wrong order (Expected > %i, Received = %i). Discarding..\n", server_counter, packet->getCounter());
						continue;
					}
				}
			}
			if(order)
				server_counter = packet->getCounter();
			if(inj && inj(packet))
				continue;
			eth->inject(packet->getPayload(), packet->getPayloadSize());
		}
	}
	delete packet;
	speed->stop();
	INFO("End of inject_thread. Finished thread\n");
	return 0;
}

void DeviceBridge::removeBridge() {
	unregister = true;
	loop = false;
	eth->breakLoop();
	//sock->closeSocket();
}
void DeviceBridge::registerCaptureCallback(CAPTURE_FUNC func) {
	cap = func;
}
void DeviceBridge::registerInjectCallback(INJECT_FUNC func) {
	inj = func;
}

const char *DeviceBridge::getLastError() {
	return 0;
}

int DeviceBridge::compareFunc(void *obj, void *item) {
	return ((DeviceInfo *)obj)->compareMAC((u_char *)item);
}

void DeviceBridge::deleteFunc(void *obj) {
	delete (DeviceInfo *)obj;
}

DeviceBridge::~DeviceBridge() {
	delete cap_packet;
	delete local_mac;
	delete remote_mac;
	delete speed;
}
