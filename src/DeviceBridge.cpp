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

PspPacket *DeviceBridge::cap_packet = 0;
u_int DeviceBridge::client_counter = 0;
u_int DeviceBridge::server_counter = 0;
u_int DeviceBridge::client_id = 0;
u_int DeviceBridge::server_id = 0;
Socket *DeviceBridge::sock = 0;
Interface *DeviceBridge::eth = 0;
List *DeviceBridge::psp_mac = 0;
List *DeviceBridge::received_mac = 0;
bool DeviceBridge::loop = true;
bool DeviceBridge::speed = true;
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
u_int DeviceBridge::mac_count = 0;

//u_int DeviceBridge::total_broadcast_received = 0;
//u_int DeviceBridge::total_broadcast_sent = 0;
//u_int DeviceBridge::total_broadcast_size_received = 0;
//u_int DeviceBridge::total_broadcast_size_sent = 0;

long DeviceBridge::last_packet = 0;

DeviceBridge::DeviceBridge(bool packet_ordering, bool packet_buffering) {
	order = packet_ordering;
	buffer = packet_buffering;
	cap_packet = new PspPacket();
	last_packet = GetTickCount();
	psp_mac = new List(compareFunc, deleteFunc);
	received_mac = new List(compareFunc, deleteFunc);
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
				th = _beginthread(&speed_thread, 4096, NULL);
#else
				pthread_create(&th, NULL, &inject_thread, NULL);
				pthread_create(&th, NULL, &speed_thread, NULL);
#endif
				//eth->compileFilter(0);
				//eth->setFilter();
				eth->captureLoop(capture_callback);
#ifdef _WIN32
				sock->WSAClean();
#endif
				eth->close();
				sock->closeSocket();
				printf("*** Client finished. Statistics of use:\n");
				printf("*** Total packets sent: %i\n", total_sent);
				printf("*** Total packets received: %i\n", total_received);
				//printf("*** Total broadcast packets sent: %i\n", total_broadcast_sent);
				//printf("*** Total broadcast packets received: %i\n", total_broadcast_received);
				printf("*** Total packets dropped: %i\n", total_droped);
				printf("*** Total bytes sent: %i\n", total_size_sent);
				printf("*** Total bytes received: %i\n", total_size_received);
				//printf("*** Total broadcast bytes sent: %i\n", total_broadcast_size_sent);
				//printf("*** Total broadcast bytes received: %i\n", total_broadcast_size_received);
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
			if(!psp_mac->exist((void *)cap_packet->getSrcMAC()) && mac_count < 16) {
				psp_mac->add(new DeviceInfo(cap_packet->getSrcMAC(), mac_count));
				printf("Registered new MAC: %s\n", cap_packet->getSrcMACstr());
				//InfoPacket *inf = new InfoPacket(cap_packet->getSrcMAC(), mac_count);
				//sock->writeSocket(inf);
				mac_count++;
			}
			if(psp_mac->exist((void *)cap_packet->getDstMAC())) {
				u_int dst_uid = 0;
				u_int src_uid = 0;
				DeviceInfo *inf = 0;
				if(cap_packet->isBroadcast()) {
					dst_uid = 0xF;
				} else {
					inf = (DeviceInfo *)received_mac->get((void *)cap_packet->getDstMAC());
					if(inf) {
						dst_uid = inf->getUID();
					} else {
						printf("Captured packet with unknown destination. Discarding\n");
						return;
					}
				}
				inf = (DeviceInfo *)psp_mac->get((void *)cap_packet->getSrcMAC());
				if(inf) {
					src_uid = inf->getUID();
				} else {
					printf("Unknown error. Source MAC isnt in list. Discarding\n");
					return;
				}
				if(sock->writeSocket((char *)cap_packet->getStrippedPacketData(dst_uid,src_uid), cap_packet->getStrippedPacketSize())) {
				//if(sock->writeSocket(cap_packet) < 0) {
					printf("capture_callback: end of stream reached. Finishing thread...\n");
					eth->breakLoop();
					loop = false;
					return;
				}
				total_sent++;
				//total_size_sent += cap_packet->getPacketSize();
				total_size_sent += cap_packet->getStrippedPacketSize();
				//if(cap_packet->isBroadcast()) {
				//	total_broadcast_sent++;
				//	total_broadcast_size_sent += cap_packet->getPacketSize();
				//}
			}
		}
	} else {
		if(unregister) {
			cap_packet->setPayload((u_char *)"END",3);
			cap_packet->setID(0);
			printf("Sending unregister packet to server\n");
			sock->writeSocket(cap_packet);
			sock->closeSocket();
		}
		printf("End of capture_callback. Finishing loop\n");
	}
}

#ifdef _WIN32
void DeviceBridge::inject_thread(void *arg) {
#else
void *DeviceBridge::inject_thread(void *arg) {
#endif
	PspPacket *packet = new PspPacket();
	int size;
	while(loop) {
		if((size = sock->readSocket(packet)) < 0) {
			printf("inject_thread: end of stream reached. Finishing thread...\n");
			loop = false;
			speed = false;
			eth->breakLoop();
			break;
		}
		total_received++;
		total_size_received += size;
		last_packet = GetTickCount();
		if(packet->checkHeader()) {
			//if(packet->isBroadcast()) {
			//	total_broadcast_received++;
			//	total_broadcast_size_received += size;
			//}
			if(!server_conn) {
				server_id = packet->getID();
				printf("Registered new server with ID: %X\n", server_id);
				server_conn = true;
			} else {
				if(server_id != packet->getID()) {
					printf("Server ID changed: %X ==> %X\n", server_id, packet->getID());
					server_id = packet->getID();
				} else {
					if(order && packet->getPacketCounter() <= server_counter) {
						//printf("Packet arrives at wrong order (Expected > %i, Received = %i). Discarding..\n", server_counter, packet->getPacketCounter());
						total_droped++;
						continue;
					}
				}
			}
			if(order)
				server_counter = packet->getPacketCounter();
			if(inj)
				if(inj(packet))
					continue;
			eth->inject(packet->getPayload(), packet->getPayloadSize());
		}
	}
	printf("End of inject_thread. Finished thread\n");
#ifndef _WIN32
	return 0;
#endif
}

void DeviceBridge::removeBridge() {
	unregister = true;
	speed = false;
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

#ifdef _WIN32
void DeviceBridge::speed_thread(void *arg) {
#else
void *DeviceBridge::speed_thread(void *arg) {
#endif
	u_int bytes = 0;
	while(speed) {
		int actual = GetTickCount();
		int last = last_packet;
		int msecs = actual - last;
		//printf("tick: %i, last: %i\n", actual, last);
		if(msecs < 0)
			msecs = -msecs;
		bytes = (total_size_received - bytes);
		printf("Download: %i bytes/s (%i kbps), Last: %i secs ago (%i msecs ago)   ", bytes, bytes/128, msecs /1000, msecs);
		bytes = total_size_received;
		fflush(stdout);
#ifdef _WIN32
		Sleep(1000);
#else
		sleep(1);
#endif
		printf("\r");
	}
#ifndef _WIN32
	return 0;
#endif
}
#ifndef _WIN32
long DeviceBridge::GetTickCount() {
	timeval tv;
	gettimeofday(&tv, NULL);
	return (tv.tv_sec * 1000) + (tv.tv_usec / 1000);
}
#endif

DeviceBridge::~DeviceBridge() {
	delete cap_packet;
	delete psp_mac;
	delete received_mac;
}
