/*
 * EthPacket.h
 *
 *  Created on: 14-ago-2008
 *      Author: code
 */

#ifndef ETHPACKET_H_
#define ETHPACKET_H_

#ifdef _WIN32
#define WINVER 0x0501 //Windows XP
#include <windows.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define PACKET_MTU_SIZE 1500

class EthPacket {
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
		u_char data[PACKET_MTU_SIZE - sizeof(eth_header)];
	};
	#pragma pack(pop)
	struct eth_data *eth;
	char src_mac_str[18];
	char dst_mac_str[18];
	static const u_char broadcast_mac[];

public:
	EthPacket(const u_char *packet_data);
	const u_char *getSrcMAC();
	const char *getSrcMACstr();
	const u_char *getDstMAC();
	const char *getDstMACstr();
	inline u_char *data() { return (u_char *)eth; }
	void setSrcMAC(const u_char *mac);
	void setDstMAC(const u_char *mac);
	bool isBroadcast();
	void hexdump();
	virtual ~EthPacket();
};

#endif /* ETHPACKET_H_ */
