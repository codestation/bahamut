#include "Interface.h"

Interface::Interface(const char *dev) {
	handle = NULL;
	this->dev = strdup(dev);
}

Interface::~Interface() {
	close();
}

bool Interface::open() {
	return (handle = pcap_open_live(dev, BUFSIZ, 1, 1000, errbuf)) != NULL;
}

int Interface::captureLoop(pcap_handler packet_func) {
	return pcap_loop ( handle, -1, packet_func, (u_char *)dev);
}

int Interface::inject(const u_char *packet_data, size_t size) {
	return pcap_sendpacket (handle, packet_data, size);
}

void Interface::breakLoop() {
	pcap_breakloop(handle);
}

int Interface::compileFilter(char *filter) {
	bpf_u_int32 pcap_netmask;
	bpf_u_int32 pcap_ip;
	if(pcap_lookupnet(dev, &pcap_ip, &pcap_netmask, errbuf) == -1)
		return -1;	
	return pcap_compile(handle, &fp, filter, 1, pcap_netmask);
}

int Interface::setFilter() {
	return pcap_setfilter(handle, &fp);
}

void Interface::close() {
	if(handle) {
		pcap_close(handle);
		handle = NULL;
	}
}
