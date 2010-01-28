/*
 *  Project Bahamut: full ad-hoc tunneling software to be used by the
 *  Playstation Portable (PSP) to emulate online features.
 *
 *  Copyright (C) 2008  Codestation
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

/*
 * File Description:
 *     Holds the packet info/data and makes easy access some packet structures
 * Special Notes:
 *		TODO: none yet
 */

#include "Packet.h"

Packet::Packet() {
	packet = new packet_container;
	packet->header.head[0] = 'M';
	packet->header.head[1] = 'H';
	packet->header.id = 0;
	packet->header.counter = 0;
	packet->header.size = sizeof(packet->data);
}
u_int Packet::getCounter() {
	return packet->header.counter;
}
int Packet::getSize() {
	return packet->header.size + 14;
}
/*
int Packet::getStrippedPacketSize() {
	return packet->header.size + 1;
}*/
int Packet::getMaxPacketSize() {
	return sizeof(packet_container);
}
void Packet::setPayloadSize(int size) {
	packet->header.size = size;
}
int Packet::getPayloadSize() {
	return packet->header.size;
}
const u_char *Packet::getPayload() {
	return (u_char *)&packet->data;
}
void Packet::setPayload(const u_char *data, size_t size) {
	memcpy(&packet->data, data, size);
	packet->header.size = size;
}
void Packet::setPayload(EthPacket *pkt, size_t size) {
	memcpy(&packet->data, pkt->data(), size);
	packet->header.size = size;
}
void Packet::setCounter(int count) {
	packet->header.counter = count;
}
u_char *Packet::getData() {
	return (u_char *)packet;
}
EthPacket *Packet::getEthData() {
	return new EthPacket(packet->data);
}

bool Packet::checkHeader() {
	return packet->header.head[0] == 'M' && packet->header.head[1] == 'H';
}

Packet::~Packet() {
	delete packet;
}
