#include "PspPacket.h"

PspPacket::PspPacket(u_char *packet_data, int lenght) {
	packet = new packet_container;
	packet->header[0] = 'M';
	packet->header[1] = 'H';
	packet->counter = 0;
	packet->size = lenght;
	memcpy(&packet->data, packet_data, lenght);
}
int PspPacket::getPacketCounter() {
	return packet->counter;
}
int PspPacket::getPacketLenght() {
	return packet->size + 10;
}
int PspPacket::getPayloadLenght() {
	return packet->size;
}
const u_char *PspPacket::getSrcMAC() {
	return packet->data.src_mac;
}
const char *PspPacket::getScrMACstr() {
	char *mac_str = new char[18];
	u_char *mac = packet->data.src_mac;
	sprintf(mac_str,"%.2x:%.2x:%.2x:%.2x:%.2x:%.2x", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
	return mac_str;
}
const u_char *PspPacket::getDstMAC() {
	return packet->data.dst_mac;
}
const char *PspPacket::getDstMACstr() {
	char *mac_str = new char[18];
	u_char *mac = packet->data.dst_mac;
	sprintf(mac_str,"%.2x:%.2x:%.2x:%.2x:%.2x:%.2x", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
	return mac_str;
}
const u_char *PspPacket::getPayload() {
	return packet->data.data;
}
void PspPacket::setSrcMAC(const u_char *mac) {
	memcpy(packet->data.src_mac, mac, 6);
}
void PspPacket::setDstMAC(const u_char *mac) {
	memcpy(packet->data.dst_mac, mac, 6);
}
void PspPacket::setPacketCounter(int count) {
	packet->counter = count;
}
const u_char *PspPacket::getPacketData() {
	return (u_char *)packet;
}
PspPacket::~PspPacket() {
	delete packet;
}
