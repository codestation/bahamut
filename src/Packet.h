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

#ifndef PACKET_H_
#define PACKET_H_

#ifdef _WIN32
#define WINVER 0x0501 //Windows XP
#include <windows.h>
#endif

#include "EthPacket.h"

#define MAX_PAYLOAD_SIZE 1486

class Packet {
private:
	#pragma pack(push,1)
	struct header_data {
		u_char head[2];
		u_short id;
		u_short counter;
		u_short size;
	};
	#pragma pack(pop)

	#pragma pack(push,1)
	struct packet_container {
		header_data header;
		u_char data[PACKET_MTU_SIZE - sizeof(header_data)];
	};
	#pragma pack(pop)

	struct packet_container *packet;
	//temporary buffer to hold the MAC string
public:
	Packet();
	u_int getCounter();
	int getSize();
	int getMaxPacketSize();
	void setPayloadSize(int size);
	const u_char *getPayload();
	int getPayloadSize();
	void setPayload(const u_char *data, size_t size);
	void setPayload(EthPacket *pkt, size_t size);
	void setCounter(int count);
	u_char *getData();
	EthPacket *getEthData();
	u_char *getStrippedPacketData(u_int dst, u_int src);
	u_short getPktType();
	bool checkHeader();
	inline void setID(u_int id) { packet->header.id = id; }
	inline u_int getID() { return packet->header.id; }
	virtual ~Packet();
};

#endif /*PACKET_H_*/
