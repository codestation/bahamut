/*
 * Packet.h
 *
 *  Created on: 14-ago-2008
 *      Author: code
 */

#ifndef PACKET_H_
#define PACKET_H_

#ifdef _WIN32
#include <windows.h>
#endif

#include <stdlib.h>
#include <string.h>

#define PACKET_MTU_SIZE 1500

class Packet {
protected:
	struct header_data {
		u_char head[2];
		u_short id;
		u_short counter;
	};
	struct packet_data {
		header_data header;
		u_char data[PACKET_MTU_SIZE - sizeof(header_data)];
	};
	struct packet_data *pkt;
public:
	Packet();
	void setID(u_short id);
	u_short getID();
	void setCounter(u_short counter);
	u_short getCounter();
	virtual ~Packet();
};

#endif /* PACKET_H_ */
