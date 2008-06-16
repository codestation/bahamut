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
*	Holds some packet info and makes easy access some packet structures
*Special Notes:
*	TODO: none yet
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

#ifndef PSPPACKET_H_
#define PSPPACKET_H_

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>

class PspPacket {
private:	
	//make the structure 1 byte-aligned (gaps in a packet are bad)
	#pragma pack(push,1)
	//simplified version of a eth packet
	struct eth_packet {
		u_char dst_mac[6];
		u_char src_mac[6];
		u_char data[1474];
	};
	#pragma pack(pop)
	
	#pragma pack(push,1)
	struct packet_container {
		u_char header[2];
		u_int id;
		u_int counter;
		u_int size;
		eth_packet data;
	};
	#pragma pack(pop)

	struct packet_container *packet;
	in_addr_t ip_addr;
	int port;
	//temporary buffer to hold the MAC string
	char mac_str[18];
public:
	PspPacket();
	u_int getPacketCounter();
	int getPacketSize();
	int getPayloadSize();
	void setPayloadSize(int size);
	const u_char *getSrcMAC();
	const char *getScrMACstr();
	char *getIPstr();
	const u_char *getDstMAC();
	const char *getDstMACstr();
	const u_char *getPayload();
	void setSrcMAC(const u_char *mac);
	void setDstMAC(const u_char *mac);
	void setPayload(const u_char *data, size_t size);
	void setPacketCounter(int count);
	u_char *getPacketData();
	bool checkHeader();
	void setIP(in_addr_t ip);
	void setPort(int port);
	in_addr_t getIP();
	int getPort();
	void setID(u_int id);
	u_int getID();
	virtual ~PspPacket();
};

#endif /*PSPPACKET_H_*/
