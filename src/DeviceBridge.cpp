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
#include "Logger.h"
#include "DeviceBridge.h"

DeviceBridge::DeviceBridge(bool packet_ordering, bool packet_buffering) {
	capture_enabled = true;
	cap = NULL;
	inj = NULL;
	client_counter = server_counter = 0;
	client_id = server_id = 0;
	sock = NULL;
	eth = NULL;
	local_mac = remote_mac = NULL;
	unregister = false;
	order = packet_ordering;
	buffer = packet_buffering;
	cap_packet = new Packet();
	//speed = new SpeedThread();
	//speed->setTime(speed->getTick());
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
	INFO("Initializing client, ID: %X\n", client_id);
}

bool DeviceBridge::makeBridge(const char *dev, const char *host, int port) {
	eth = new Interface(dev);
	sock = new Socket(host, port);
	// doesnt return until connection end
	bool res = makeBridge(eth, sock);
	delete eth;
	delete sock;
	return res;
}
bool DeviceBridge::makeBridge(Interface *eth, Socket *sock) {
	if(eth->open()) {
		if(sock->connectSocket(Socket::UDP_SOCKET)) {
			char buffer_data[128];
			INFO("Start of inject_thread. Starting thread\n");
			this->start();
			//speed->start();
			sprintf(buffer_data, "not ether src %s", eth->getMacAddressStr());
			//const char *mac = eth->getMacAddressStr();
			//sprintf(buffer_data, "not ether src %s and not ether dst %s", mac, mac);
			eth->compileFilter(buffer_data);
			eth->setFilter();
			INFO("Filter applied: %s\n", buffer_data);
			eth->setdirection();
			// starts loop, doesnt return until close
			INFO("Start of capture_callback. Starting loop\n");
			eth->captureLoop(capture_callback, (u_char *)this);
			INFO("End of capture_callback. Finished loop\n");
			this->stop();
			this->wait();
			eth->close();
			sock->closeSocket();
			return true;
		}
	}
	return false;
}

void DeviceBridge::capture_callback(u_char *user, const struct pcap_pkthdr* packet_header, const u_char* packet_data) {
	((DeviceBridge *)user)->capture(packet_header, packet_data);
}

void DeviceBridge::capture(const struct pcap_pkthdr* packet_header, const u_char* packet_data) {
	if(!capture_enabled)
		return;
	if(packet_header->len <= MAX_PAYLOAD_SIZE) {
		if(cap && cap(packet_data, packet_header->len))
			return;
		EthPacket eth_packet(packet_data);
		cap_packet->setPayload(&eth_packet, packet_header->len);
		cap_packet->setCounter(client_counter++);
		cap_packet->setID(client_id);
		if(!local_mac->exist((void *)eth_packet.getSrcMAC()) && local_mac->count() < 16) {
			local_mac->add(new DeviceInfo(eth_packet.getSrcMAC(), local_mac->count()));
			INFO("Registered new MAC: %s\n", eth_packet.getSrcMACstr());
		}
		if(local_mac->exist((void *)eth_packet.getDstMAC())) {
			INFO("Packet destination is local, ignoring\n");
			return;
		}
		if(!eth_packet.isBroadcast()) {
			if(remote_mac->empty()) {
				INFO("Remote list empty. Discarding\n");
				return;
			}
			if(!remote_mac->exist((void *)eth_packet.getDstMAC())) {
				INFO("Captured packet with unknown destination. Discarding\n");
				return;
			}
		}
		DEBUG("SRC: %s, DST: %s\n", eth_packet.getSrcMACstr(), eth_packet.getDstMACstr());
		if(sock->sendData(cap_packet->getData(), cap_packet->getSize()) < 0) {
			//INFO("capture_callback: end of stream reached. Finishing thread...\n");
			//eth->breakLoop();
			//ignoreCapture();
			//return;
			ERR("Error writing in socket\n");
		}
	}
}

void DeviceBridge::unregisterClient() {
	cap_packet->setPayload((u_char *)"END",3);
	cap_packet->setID(0);
	INFO("Sending unregister packet to server\n");
	sock->sendData(cap_packet->getData(), cap_packet->getSize());
	sock->closeSocket();
}

int DeviceBridge::run() {
	Packet *packet = new Packet();
	int size;
	int server_conn = false;
	while(capture_enabled) {
		if((size = sock->receiveData(packet->getData(), packet->getMaxPacketSize())) == -1) {
			if(!sock->readAgain()) {
				ERR("Error while reading socket, server offline?\n");
				//capture_enabled = false;
				//eth->breakLoop();
				//speed->stop();
				//break;
				continue;
			} else {
				continue;
			}
		}
		//speed->addSize(size);
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
			if(!remote_mac->exist((void *)packet->getEthData()->getSrcMAC())) {
				remote_mac->add(new DeviceInfo(packet->getEthData()->getSrcMAC(), remote_mac->count()));
				INFO("Registered new remote MAC: %s\n", packet->getEthData()->getSrcMACstr());
			}
			if(order)
				server_counter = packet->getCounter();
			if(inj && inj(packet))
				continue;
			eth->inject(packet->getPayload(), packet->getPayloadSize());
		}
	}
	delete packet;
	INFO("End of inject_thread. Finished thread\n");
	return 0;
}

void DeviceBridge::removeBridge() {
	unregister = true;
	//speed->stop();
	this->stop();
	this->wait();
	eth->breakLoop();
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

int DeviceBridge::compareFunc(void *obj, void *item) {
	return ((DeviceInfo *)obj)->compareMAC((u_char *)item);
}

void DeviceBridge::deleteFunc(void *obj) {
	delete (DeviceInfo *)obj;
}

void DeviceBridge::stop() {
	capture_enabled = false;
}

DeviceBridge::~DeviceBridge() {
	delete cap_packet;
	delete local_mac;
	delete remote_mac;
	//delete speed;
}
