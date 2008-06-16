/*
*Project Name: Bahamut
* 
*Programmers: Codestation, ZackVixACD
* 
*Project Descrption: The project bahamut is a full ad-hoc tunneling
*	software to be used by the Playstation Portable (PSP) to emulate 
*	online features.
*
*File Description:
*	none yet
*Special Notes:
*	none yet
*
*Copyright Stuff:
*   This program is free software: you can redistribute it and/or modify
*   it under the terms of the GNU General Public License as published by
*   the Free Software Foundation, either version 3 of the License, or
*   (at your option) any later version.
* 
*   This program is distributed in the hope that it will be useful,
*   but WITHOUT ANY WARRANTY; without even the implied warranty of
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*   GNU General Public License for more details.
* 
*  You should have received a copy of the GNU General Public License
*   along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <stdlib.h>
#include <pthread.h>
#include "Socket.h"
#include "Interface.h"
#include "PspPacket.h"
#include "DeviceInfo.h"
#include "DeviceContainer.h"
#include "UDPServer.h"
pthread_t th;
Socket *sock = NULL;
Interface *interf = NULL;
UDPServer *serv = NULL;
bool flag = false;
DeviceContainer *src_con = NULL;
DeviceContainer *dst_con = NULL;
PspPacket *cap_packet;

void capture_callback(u_char* user, const struct pcap_pkthdr* packet_header, const u_char* packet_data) {
	cap_packet->setPayload(packet_data, packet_header->len);
	if(!flag) {
		DeviceInfo *info = src_con->getDevice(cap_packet->getSrcMAC());
		if(info == NULL) {
			if(src_con->deviceCount() < 4) {
				info = new DeviceInfo(cap_packet->getSrcMAC());
				info->setRandomID();
				src_con->addDevice(info);
				printf("Registered new source device, MAC: %s, ID: %X\n", cap_packet->getScrMACstr(), info->getID());
			} else {
				printf("Cant add more than 4 source devices: %s\n", cap_packet->getScrMACstr());
			}			
		} else {
			info->setPacketCounter(info->getPacketCounter() + 1);
			cap_packet->setPacketCounter(info->getPacketCounter());
		}
		cap_packet->setID(info->getID());
		if(sock->writeSocket(cap_packet) < 0) {
			printf("Conection closed by host. Terminating capturing thread...\n");
			flag = true;
		}
	}
}

void *inject_thread(void *arg) {
	PspPacket *packet = new PspPacket();
	while(!flag) {
		if(sock->readSocket(packet) < 0) {
			printf("Conection closed by host. Terminating injecting thread...\n");
			flag = true;
			interf->breakLoop();
			break;
		}
		if(packet->checkHeader()) {
			DeviceInfo *dev = dst_con->getDevice(packet->getSrcMAC());
			if(dev == NULL) {
				if(dst_con->deviceCount() < 4) {
					dev = new DeviceInfo(packet->getSrcMAC());
					dst_con->addDevice(dev);
					dev->setID(packet->getID());
					printf("Registered new remote device, MAC: %s, ID %X\n", packet->getScrMACstr(), packet->getID());
					printf("Updating filtering rules... ");
					switch(interf->updateFilters(dst_con)) {
					case 0:
						printf("done\n");
						break;
					case -1:
						printf("error while compiling filter\n");
						break;
					case -2:
						printf("error while applying filter\n");
					}
				} else {
					printf("Cant add more than 4 remote devices: %s\n", packet->getScrMACstr());
					continue;
				}
			} else {
				if(dev->getID() != packet->getID()) {
					printf("ID has changed (OLD: %X, NEW: %X), updating...\n", dev->getID(), packet->getID());
					dev->setID(packet->getID());
				} else {
					if(packet->getPacketCounter() <= dev->getPacketCounter()) {
						printf("Server: Packet arrives at wrong order (Expected > %i, Received = %i). Discarding..\n", dev->getPacketCounter(), packet->getPacketCounter());
						continue;
					}
				}
			}
			dev->setPacketCounter(packet->getPacketCounter());
			interf->inject(packet->getPayload(), packet->getPayloadSize());
		}
	}
	return 0;
}

//TODO remove this function, or integrate with the interface class
char *select_adapter() {
	pcap_if_t *alldevs;
	pcap_if_t *d;
	char errbuf[PCAP_ERRBUF_SIZE];
	int i = 0;
	int inum;

	/* Retrieve the device list */
	if (pcap_findalldevs(&alldevs, errbuf) == -1) {
		printf("Error in pcap_findalldevs: %s\n", errbuf);
		return NULL;
	}

	/* Print the list */
	printf("Select your wireless interface from the list:\n");
	for (d=alldevs; d; d=d->next) {
		printf("%d. %s", ++i, d->name);
		if (d->description)
			printf(" (%s)\n", d->description);
		else
			printf(" (No description available)\n");
	}

	if (i==0) {
		printf("No interfaces found! Make sure libpcap/WinPcap is installed.\n");
		return NULL;
	}

	printf("Enter the interface number (1-%d):", i);
	scanf("%d", &inum);

	if (inum < 1 || inum> i)
	{
		printf("Interface number out of range.\n");
		/* Free the device list */
		pcap_freealldevs(alldevs);
		return NULL;
	}

	/* Jump to the selected adapter */
	for(d=alldevs, i=0; i< inum-1;d=d->next, i++);
	return d->name;
}

int main(int argc, char ** argv) {
	if(argc != 3) {
		printf("Usage: bahamuth <host> <port>\n");
		return 1;
	}
	printf("Bahamut engine SVN (prototype) starting...\n");
	srand(time(0));
	char *dev = select_adapter();
	if(dev != NULL) {
		interf = new Interface(dev);
		printf("Opening interface: %s\n", dev);
		if(interf->open()) {
			
			if(strcmp(argv[1], "localhost") == 0) {
				printf("Creating UDP server...\n");
				serv = new UDPServer(atoi(argv[2]));
				printf("Starting UDP server...\n");
				serv->start();
			}
			
			sock = new Socket(argv[1],atoi(argv[2]),"udp");
			printf("Trying to connect to %s:%s\n", argv[1], argv[2]);
			if(sock->connectSocket()) {
				printf("Connection established to %s:%s\n", argv[1], argv[2]);
				src_con = new DeviceContainer();
				dst_con = new DeviceContainer();
				printf("Adding default filters...");
				switch(interf->updateFilters(dst_con)) {
				case 0:
					printf("done\n");
					break;
				case -1:
					printf("error while compiling filter\n");
					break;
				case -2:
					printf("error while applying filter\n");
				}
				printf("Starting inject thread...\n");
				pthread_create(&th, NULL, &inject_thread, NULL);
				printf("Starting capture thread...\n");
				cap_packet = new PspPacket();
				interf->captureLoop(&capture_callback);
				delete cap_packet;
				printf("Closing connection with host\n");
				sock->closeSocket();
			} else {
				printf("Cannot connect to host :(\n");
			}
			printf("Closing interface: %s\n", dev);
			interf->close();				
		} else {
			printf("Cannot open device\n");
		}
	}
	printf("Bye ~nya!\n");
	delete sock;
	delete interf;
	if(strcmp(argv[1], "localhost") == 0) {
		serv->stop();
		delete serv;
	}
	return 0;
}
