#include "UDPServer.h"

bool UDPServer::loop_flag = true;

UDPServer::UDPServer(int port) {
	this->port = port;
}

void *UDPServer::run(void *arg) {
	ServerSocket *sock = new ServerSocket(*(int *)arg, "udp");
	sock->bindSocket();
	PspPacket *packet = new PspPacket();
	DeviceContainer *dev_list = new DeviceContainer();
	while(loop_flag) {
		sock->receive(packet);
		//I KNOW, recycled code from main.cpp, needs to find a way to merge
		if(packet->checkHeader()) {
			DeviceInfo *dev = dev_list->getDevice(packet->getIP(), packet->getPort());
			if(dev == NULL) {
				if(dev_list->deviceCount() < 4) {
					dev = new DeviceInfo(packet->getIP(), packet->getPort());
					dev->setID(packet->getID());
					dev_list->addDevice(dev);
					printf("Server: Registered new device from %s:%i, ID: %X\n", dev->getIPstr(), dev->getPort(), dev->getID());
				} else {
					printf("Server: Cant add more than 4 clients, %s:%i dropped.\n", packet->getIPstr(), packet->getPort());
					continue;
				}
			} else {
				if(dev->getID() != packet->getID()) {
					printf("Server: ID has changed (OLD: %X, NEW: %X), updating...\n", dev->getID(), packet->getID());
					dev->setID(packet->getID());
				} else {
					if(packet->getPacketCounter() <= dev->getPacketCounter()) {
						printf("Server: Packet arrives at wrong order (Expected > %i, Received = %i). Discarding..\n", dev->getPacketCounter(), packet->getPacketCounter());
						continue;
					}
				}
				dev->setPacketCounter(packet->getPacketCounter());
			}
			dev->setPacketCounter(packet->getPacketCounter());
			in_addr_t dest_ip = packet->getIP();
			int dest_port = packet->getPort();
			for(int i = 0;i < 4; i++) {
				DeviceInfo *info = dev_list->getDeviceAtPos(i);
				if(info) {
					if(info->getIP() != dest_ip || info->getPort() != dest_port) {
						packet->setIP(info->getIP());
						packet->setPort(info->getPort());
						sock->send(packet);
					}
				}
			}
		} else {
			printf("Server: Unknown packet from %s:%i. Discarding...\n", packet->getIPstr(), packet->getPort());
		}
	}
	delete packet;
	delete dev_list;
	delete sock;
	return 0;
}

void UDPServer::start() {
	pthread_create(&th, NULL, &run, &port);	
}

void UDPServer::stop() {
	loop_flag = false;
}

UDPServer::~UDPServer() {
	
}
