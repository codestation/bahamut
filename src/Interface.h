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
*	Network interface functions
*Special Notes:
*	TODO: use the native functions for every platform
* 		: make a tun/tap alternative
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

#ifndef INTERFACE_H_
#define INTERFACE_H_

#include <pcap.h>
#include <string.h>
#include "DeviceContainer.h"

#define PSP_FAT_MAC_RULE  "(ether[0] = 0x00 && ether[1] = 0x16 && ether[2] = 0xFE)"
#define PSP_SLIM_MAC_RULE "(ether[0] = 0x00 && ether[1] = 0x1C && ether[2] = 0x26)"
#define PSP_N_SM_MAC_RULE "(ether[0] = 0x00 && ether[1] = 0x1D && ether[2] = 0xD9)"

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
	int updateFilters(DeviceContainer *cont);
	char *initializeFilter(char *buffer);
	int setFilter();
	void breakLoop();
	virtual ~Interface();
};

#endif /*INTERFACE_H_*/
