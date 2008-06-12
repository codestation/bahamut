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
	//u_char *packet_data;
public:
	PspPacket();
	int getPacketCounter();
	int getPacketSize();
	int getPayloadSize();
	void setPayloadSize(int size);
	const u_char *getSrcMAC();
	const char *getScrMACstr();
	const u_char *getDstMAC();
	const char *getDstMACstr();
	const u_char *getPayload();
	void setSrcMAC(const u_char *mac);
	void setDstMAC(const u_char *mac);
	void setPayload(const u_char *data, size_t size);
	void setPacketCounter(int count);
	u_char *getPacketData();
	bool checkHeader();
	virtual ~PspPacket();
};

#endif /*PSPPACKET_H_*/
