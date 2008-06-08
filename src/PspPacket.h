#ifndef PSPPACKET_H_
#define PSPPACKET_H_

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

class PspPacket {
private:
	
	//make the structure 1 byte-aligned (gaps in a packet are bad)
	#pragma pack(push,1)
	//simplified version of a eth packet
	struct eth_packet {
		u_char dst_mac[6];
		u_char src_mac[6];
		u_char data[1478];
	};
	#pragma pack(pop)
	
	#pragma pack(push,1)
	struct packet_container {
		u_char header[2];
		u_int counter;
		u_int size;
		eth_packet data;
	};
	#pragma pack(pop)

	struct packet_container *packet;
	u_char *packet_data;
public:
	PspPacket(u_char *packet_data, int lenght);
	int getPacketCounter();
	int getPacketLenght();
	int getPayloadLenght();
	const u_char *getSrcMAC();
	const char *getScrMACstr();
	const u_char *getDstMAC();
	const char *getDstMACstr();
	const u_char *getPayload();
	void setSrcMAC(const u_char *mac);
	void setDstMAC(const u_char *mac);
	void setPacketCounter(int count);
	const u_char *getPacketData();
	virtual ~PspPacket();
};

#endif /*PSPPACKET_H_*/
