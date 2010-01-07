/*
 * EthPacket.cpp
 *
 *  Created on: 14-ago-2008
 *      Author: code
 */

#include "EthPacket.h"

const u_char EthPacket::broadcast_mac[] = {(u_char)0xFF, (u_char)0xFF, (u_char)0xFF, (u_char)0xFF, (u_char)0xFF, (u_char)0xFF};

EthPacket::EthPacket() {
	::Packet();
	eth = (eth_data *)pkt->data;
	memset(&eth->header, 0, sizeof(eth_header));
}

const u_char *EthPacket::getSrcMAC() {
	return eth->header.src_mac;
}
const char *EthPacket::getSrcMACstr() {
	u_char *mac = eth->header.src_mac;
	sprintf(src_mac_str,"%.2x:%.2x:%.2x:%.2x:%.2x:%.2x", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
	return src_mac_str;
}
const u_char *EthPacket::getDstMAC() {
	return eth->header.dst_mac;
}
const char *EthPacket::getDstMACstr() {
	u_char *mac = eth->header.dst_mac;
	sprintf(dst_mac_str,"%.2x:%.2x:%.2x:%.2x:%.2x:%.2x", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
	return dst_mac_str;
}

void EthPacket::setSrcMAC(const u_char *mac) {
	memcpy(eth->header.src_mac, mac, sizeof(eth->header.src_mac));
}
void EthPacket::setDstMAC(const u_char *mac) {
	memcpy(eth->header.dst_mac, mac, sizeof(eth->header.dst_mac));
}
bool EthPacket::isBroadcast() {
	return !memcmp(getDstMAC(), broadcast_mac, 6);
}

EthPacket::~EthPacket() {
	eth = NULL;
}
