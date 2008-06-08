#include "PspInfo.h"

PspInfo::PspInfo(u_char *psp_mac) {
	count = 0;
	memcpy(mac, psp_mac, 6);
}
const u_char *PspInfo::getMAC() {
	return mac;
}
void PspInfo::setMAC(const u_char *psp_mac) {
	memcpy(mac, psp_mac, 6);
}
int PspInfo::getPacketCounter() {
	return count;
}
void PspInfo::setPacketCounter(int newcount) {
	count = newcount;
}
PspInfo::~PspInfo() {
	
}
