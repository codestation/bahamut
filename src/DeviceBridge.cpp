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
 *     Sends the capture packets to a main server, then receive the
 *     packets of the other clients and injects on the interface
 *
 * Special Notes:
 *     TODO: get rid of all those static things :/
 */

#include "DeviceBridge.h"

PspPacket *DeviceBridge::cap_packet = 0;
u_int DeviceBridge::client_counter = 0;
u_int DeviceBridge::server_counter = 0;
u_int DeviceBridge::client_id = 0;
u_int DeviceBridge::server_id = 0;
Socket *DeviceBridge::sock = 0;
Interface *DeviceBridge::eth = 0;
bool DeviceBridge::loop = true;
DeviceBridge::CAPTURE_FUNC DeviceBridge::cap = 0;
DeviceBridge::INJECT_FUNC DeviceBridge::inj = 0;

DeviceBridge::DeviceBridge() {
	cap_packet = new PspPacket();
	srand(time(0));
	client_id = rand();
}

bool DeviceBridge::makeBridge(const char *dev, const char *host, int port) {
	eth = new Interface(dev);
	sock = new Socket(host, port, "udp");
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
#ifdef _WIN32
				th = _beginthread(&inject_thread, 4096, NULL);
#else
				pthread_create(&th, NULL, &inject_thread, NULL);
#endif
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
			if(cap)
				if(cap(packet_data, packet_header->len))
					return;
			cap_packet->setPayload(packet_data, packet_header->len);
			cap_packet->setPacketCounter(client_counter++);
			cap_packet->setID(client_id);
			if(sock->writeSocket(cap_packet) < 0) {
				printf("capture_callback: end of stream reached. Finishing thread...\n");
				eth->breakLoop();
				loop = false;
			}
		}
	}
}

#ifdef _WIN32
void DeviceBridge::inject_thread(void *arg) {
#else
void *DeviceBridge::inject_thread(void *arg) {
#endif
	PspPacket *packet = new PspPacket();
	while(loop) {
		if(sock->readSocket(packet) < 0) {
			printf("inject_thread: end of stream reached. Finishing thread...\n");
			loop = false;
			eth->breakLoop();
			break;
		}
		if(packet->checkHeader()) {
			if(server_id != packet->getID()) {
				printf("Server ID changed: %X ==> %X\n", server_id, packet->getID());
				server_id = packet->getID();
			} else {
				if(packet->getPacketCounter() <= server_counter) {
					printf("Packet arrives at wrong order (Expected > %i, Received = %i). Discarding..\n", server_counter, packet->getPacketCounter());
					continue;
				}
			}
			server_counter = packet->getPacketCounter();
			if(inj)
				if(inj(packet))
					continue;
			eth->inject(packet->getPayload(), packet->getPayloadSize());
		}
	}
#ifndef _WIN32
	return 0;
#endif
}

void DeviceBridge::removeBridge() {
	loop = false;
	eth->breakLoop();
	eth->close();
	sock->closeSocket();
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

DeviceBridge::~DeviceBridge() {
	delete cap_packet;
}
