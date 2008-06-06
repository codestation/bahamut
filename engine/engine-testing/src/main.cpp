#include <stdlib.h>
#include <pthread.h>
#include "Socket.h"
#include "Interface.h"

#define ETHER_ADDR_LEN	6

Socket *sock = NULL;
Interface *interf = NULL;
u_char buffer[2048];
u_int buffer_offset = 0;
u_int buffer_end = 0;
pthread_t th;
bool flag = false;
u_int count = 0;
u_char ether_shost[4][ETHER_ADDR_LEN];
u_char ether_dhost[4][ETHER_ADDR_LEN];
int used_src_eth = 0;
int used_dst_eth = 0;

//u_char mac_a[6] = {1,2,3,4,5,6};
//u_char mac_b[6] = {6,5,4,3,2,1};
//bool mac = false;

bool is_registered(const u_char *packet) {
	bool found = false;
	int i = 0;
	while(i < used_src_eth && !found) {
		if(memcmp(ether_shost[i], &packet[ETHER_ADDR_LEN], ETHER_ADDR_LEN) == 0){
			found = true;
		} else {
			i++;
		}
	}
	return found;
}

void update_filters() {
	char str[1024];
	char mac[32];
	strcpy(str, "not (ether src ");
	for(int i = 0;i < used_dst_eth;i++) {
		sprintf(mac,"%.2x:%.2x:%.2x:%.2x:%.2x:%.2x)",ether_dhost[i][0],ether_dhost[i][1],ether_dhost[i][2],ether_dhost[i][3],ether_dhost[i][4],ether_dhost[i][5]);
		strcat(str,mac);
		if(i < (used_dst_eth-1)) {
			strcat(str, " and not (ether src ");
		}
	}
	printf("Compiled rule: %s\n", str);
	if(interf->compileFilter(str) != -1) {
		if(interf->setFilter() == -1) {
			printf("Error while set the network filter\n");
		}
	} else {
		printf("Error while compile the network filter\n");
	}
}

void register_mac(const u_char *packet) {
	if(!is_registered(packet)) {
		if(used_src_eth < 4) {
			memcpy(ether_shost[used_src_eth],&packet[ETHER_ADDR_LEN], ETHER_ADDR_LEN);
			printf("Registered source PSP with MAC: %.2x:%.2x:%.2x:%.2x:%.2x:%.2x\n", ether_shost[used_src_eth][0], ether_shost[used_src_eth][1], ether_shost[used_src_eth][2], ether_shost[used_src_eth][3], ether_shost[used_src_eth][4], ether_shost[used_src_eth][5]);
			used_src_eth++;
		} else {
			printf("Cant register more source devices, sorry\n");
		}
	}
}


bool is_dst_registered(const u_char *packet) {
	bool found = false;
	int i = 0;
	while(i < used_dst_eth && !found) {
		if(memcmp(ether_dhost[i], &packet[ETHER_ADDR_LEN], ETHER_ADDR_LEN) == 0){
			found = true;
		} else {
			i++;
		}
	}
	return found;
}

void register_dst_mac(const u_char *packet) {
	if(!is_dst_registered(packet)) {
		if(used_dst_eth < 4) {
			memcpy(ether_dhost[used_dst_eth],&packet[ETHER_ADDR_LEN], ETHER_ADDR_LEN);
			printf("Registered remote PSP with MAC: %.2x:%.2x:%.2x:%.2x:%.2x:%.2x\n", ether_dhost[used_dst_eth][0], ether_dhost[used_dst_eth][1], ether_dhost[used_dst_eth][2], ether_dhost[used_dst_eth][3], ether_dhost[used_dst_eth][4], ether_dhost[used_dst_eth][5]);
			used_dst_eth++;
			update_filters();
		} else {
			printf("Cant register more remote devices, sorry\n");
		}
	}
}

void packet_callback(u_char* user, const struct pcap_pkthdr* packet_header, const u_char* packet_data) {
	if(!flag) {
		//u_char *packet_dat = new u_char[packet_header->len];
		//memcpy(packet_dat, packet_data, packet_header->len);
		//if(mac) {
		//	memcpy(packet_dat+6,mac_a,6);
		//}
		register_mac(packet_data);
		//printf("<<<<< Captured packet of %'4i bytes, sending to server...\n", packet_header->len);
		if(sock->writeSocket(packet_data, packet_header->len) < 0) {
			printf("Conection closed by host. Terminating capturing thread...\n");
			flag = true;
			//interf->breakLoop();
			//sock->closeSocket();
		}
	}
}

void *thread_loop(void *arg) {
	while(!flag) {
		//printf("Waiting for server frame...\n");
		int size = sock->readSocket(buffer + buffer_offset, sizeof(buffer) - buffer_offset);
		if(size < 0) {
			printf("Conection closed by host. Terminating injecting thread...\n");
			flag = true;
			interf->breakLoop();
			//sock->closeSocket();
			break;
		}
		//printf(">>>>> Received frame from server of %'4i bytes\n", size);
		buffer_end = buffer_offset + size;
		buffer_offset = 0;
		while(true) {
			if(buffer_offset > (buffer_end - 10)) {
				//printf("End of buffer reached\n");
				break;
			}
			if(buffer[buffer_offset] == 'M' && buffer[buffer_offset+1] == 'H') {
				//printf("PSP packet recognized\n");
				buffer_offset += 2;
				if(*(u_int *)(buffer+buffer_offset) >= count) {
					printf("Packet arrives at wrong order. Discarding...\n");
					buffer_offset += 4;
					buffer_offset += *(int *)(buffer+buffer_offset);
					continue;
				}
				count = *(u_int *)(buffer+buffer_offset);
				buffer_offset += 4;
				u_int len = *(int *)(buffer+buffer_offset);
				//printf("PSP packet lenght: %'4i bytes\n", len);
				buffer_offset += 4;
				if(len <= (buffer_end - buffer_offset)) {
					register_dst_mac(buffer + buffer_offset);
					//printf("Injecting packet to interface...\n");
					if(interf->inject(buffer + buffer_offset, len) == -1) {
						printf("Error while injecting packet\n");
					}
					buffer_offset += len;
				} else {
					//printf("Incomplete packet, more data is needed\n");
					buffer_offset -= 10;
					break;
				}
			} else {
				printf("Unknown packet. Discarding...\n");
				buffer_end = buffer_offset;
				break;
			}
		}
		u_int block_size = buffer_end - buffer_offset;
		if(block_size > 0) {
			//printf("Buffer not empty, %i bytes left, reallocating frame...\n", block_size);
			u_char *temp = new u_char[block_size];
			memcpy(temp, buffer + buffer_offset, block_size);
			memcpy(buffer, temp, block_size);
			delete temp;
		}
		buffer_offset = block_size;	
	}
	return 0;
}

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
	if(argc != 4) {
		printf("Usage: bahamuth <host> <port> <(tcp|udp)>\n");
		return 1;
	}
	//if(strcmp(argv[1],"localhost") == 0) {
	//   mac = true;
	//}
	printf("Bahamuth client v0.0.4 (prototype) starting...\n");
	char *dev = select_adapter();
	if(dev != NULL) {
		interf = new Interface(dev);
		printf("Opening interface: %s\n", dev);
		if(interf->open()) {
			sock = new Socket(argv[1],atoi(argv[2]), argv[3]);
			printf("Trying to connect to %s:%s\n", argv[1], argv[2]);
			if(sock->connectSocket()) {
				printf("Connection established to %s:%s\n", argv[1], argv[2]);
				interf->setFilter();
				printf("Starting inject thread...\n");
				pthread_create(&th, NULL, &thread_loop, NULL);
				printf("Starting capture thread...\n");
				interf->captureLoop(&packet_callback);
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
	return 0;
}
