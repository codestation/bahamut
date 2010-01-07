/*
 * Packet.cpp
 *
 *  Created on: 14-ago-2008
 *      Author: code
 */

#include "Packet.h"

Packet::Packet() {
	pkt = new packet_data;
	pkt->header.head[0] = 'M';
	pkt->header.head[1] = 'H';
	pkt->header.id = 0;
	pkt->header.counter = 0;
	memset(pkt->data, 0, sizeof(pkt->data));
}

void Packet::setID(u_short id) {
	pkt->header.id = id;
}

u_short Packet::getID() {
	return pkt->header.id;
}

void Packet::setCounter(u_short counter) {
	pkt->header.counter = counter;
}

u_short Packet::getCounter() {
	return pkt->header.counter;
}

Packet::~Packet() {
	delete pkt;
}
