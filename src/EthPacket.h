/*
 * EthPacket.h
 *
 *  Created on: 14-ago-2008
 *      Author: code
 */

#ifndef ETHPACKET_H_
#define ETHPACKET_H_

#include <stdio.h>
#include "Packet.h"

class EthPacket : public Packet {
private:
	#pragma pack(push,1)
	struct eth_header {
		u_char dst_mac[6];
		u_char src_mac[6];
		u_char type[2];
	};
	#pragma pack(pop)

	#pragma pack(push,1)
	struct eth_data {
		eth_header header;
		u_char data[PACKET_MTU_SIZE - sizeof(header_data) - sizeof(eth_header)];
	};
	#pragma pack(pop)
	struct eth_data *eth;

	char src_mac_str[18];
	char dst_mac_str[18];
	static const u_char broadcast_mac[];

public:
	EthPacket();
	const u_char *getSrcMAC();
	const char *getSrcMACstr();
	const u_char *getDstMAC();
	const char *getDstMACstr();
	void setSrcMAC(const u_char *mac);
	void setDstMAC(const u_char *mac);
	bool isBroadcast();
	virtual ~EthPacket();
};

#endif /* ETHPACKET_H_ */
