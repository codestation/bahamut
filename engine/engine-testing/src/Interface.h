#ifndef INTERFACE_H_
#define INTERFACE_H_

#include <pcap.h>
#include <string.h>

class Interface {
private:
	char *dev;
	pcap_t *handle;
	pcap_pkthdr* packet_header;
	char errbuf[PCAP_ERRBUF_SIZE];
	struct bpf_program fp;

public:
	Interface(const char *dev);
	bool open();
	void close();
	int captureLoop(pcap_handler packet_func);
	int capture(const void *packet_data, size_t size);
	int inject(const u_char *packet_data, size_t size);
	int compileFilter(char *filter);
	int setFilter();
	void breakLoop();
	virtual ~Interface();
};

#endif /*INTERFACE_H_*/
