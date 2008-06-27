/*
 *  Project Bahamut: full ad-hoc tunneling software to be used by the
 *  Playstation Portable (PSP) to emulate online features.
 *
 *  Copyright (C) 2008  Project Bahamut team
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
 *     Network interface functions
 * Special Notes:
 *		TODO: use the native functions for every platform
 * 			: make a tun/tap alternative
 */

#ifndef INTERFACE_H_
#define INTERFACE_H_

#ifdef _WIN32
#define HAVE_REMOTE
#endif
#include <pcap.h>
#include <string.h>
#include "InterfaceInfo.h"
#include "List.h"

class Interface {
private:
	char *dev;
	pcap_t *handle;
	pcap_pkthdr* packet_header;
	char errbuf[PCAP_ERRBUF_SIZE];
	struct bpf_program fp;
	static void delete_info(void *);
public:
	Interface(const char *dev);
	bool open();
	void close();
	int captureLoop(pcap_handler packet_func);
	int capture(const void *packet_data, size_t size);
	int inject(const u_char *packet_data, size_t size);
	int compileFilter(char *filter);
	static List *getAdapterList();
	//int updateFilters(DeviceContainer *cont);
	int setFilter();
	void breakLoop();
	virtual ~Interface();
};

#endif /*INTERFACE_H_*/
