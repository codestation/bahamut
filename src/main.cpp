/*
 *  Project Bahamut: full ad-hoc tunneling software to be used by the
 *  Playstation Portable (PSP) to emulate online features.
 *
 *  Copyright (C) 2008-2010  Codestation
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
 *     Startup file
 * Special Notes:
 *     TODO: more cleanup, remove all the possible code and make some API
 *     		 to communicate with the UI
 */

#ifdef _WIN32
#define WINVER 0x0501 //Windows XP
#include <windows.h>
#endif
#include <stdlib.h>
#include <signal.h>
#include "Interface.h"
#include "UDPServer.h"
#include "DeviceBridge.h"
#include "Logger.h"
#include "ArgParser.h"

UDPServer *serv = NULL;
DeviceBridge *bridge = NULL;

void exit_signal(int signal) {
	INFO("\nTrapped CTRL+C signal, shutting down...\n");
	if(bridge) {
		bridge->removeBridge();
	}
	serv->stop();
}

//int capture_func(const u_char *mac, u_int len) {
//	u_int manufacturer = (mac[6] << 16) | (mac[7] << 8) | mac[8];
//	return (manufacturer == 0x001C26 || manufacturer == 0x001DD9 || manufacturer == 0x0016FE) ? 0 : 1;
//	return (manufacturer == 0x001C26 || manufacturer == 0x001DD9 || manufacturer == 0x00197D) ? 0 : 1;
//}


//int inject_func(PspPacket *packet) {
//	total_received++;
//	return 0;
//}

int main(int argc, char ** argv) {
	ArgParser opts;
	if(!opts.parse(argc, argv)) {
		ERR("Usage: bahamuth-engine -h <host> -p <port>  [-i <interface>] [-l] [-o]\n");
		return 1;
	}
	if(opts.verboseMode())
		INFO_ON();

	if(opts.listInterfaces()) {
		INFO_ON();
		List *lst = Interface::getAdapterList();
		if(!lst) {
			ERR("Cant get the interface list. Exiting...\n");
			return 1;
		}
		int i = 0;
		INFO("Interface list:\n");
		while(lst->hasNext()) {
			InterfaceInfo *inf = (InterfaceInfo *)lst->next();
//#ifdef _WIN32
//			INFO("%i) %s\n", i++, inf->desc);
//#else
			INFO("%i) %s (%s)\n", i++, inf->name, inf->desc);
//#endif
		}
		delete lst;
		return 2;
	}
	INFO("Bahamut engine SVN r%s starting...\n", SVN_REV);
	if(!opts.getHost()) {
		ERR("No host defined, exiting...\n");
		return 1;
	}
	if(!opts.getPort()) {
		ERR("No port defined, exiting...\n");
		return 1;
	}
	if(!opts.interfaceName() && !opts.dedicatedServer()) {
			ERR("No interface defined, exiting...\n");
			return 1;
	}

	INFO("Intalling signal handler...\n");
	signal(SIGINT, exit_signal);

	if(strcmp(opts.getHost(), "localhost") == 0) {
		INFO("Creating UDP server...\n");
		serv = new UDPServer(atoi(opts.getPort()), !opts.disableOrdering());
		INFO("Starting UDP server...\n");
		serv->start();
	}
	if(!opts.dedicatedServer()) {
		bridge = new DeviceBridge(!opts.disableOrdering());
		//bridge->registerCaptureCallback(capture_func);
		//bridge->registerInjectCallback(inject_func);
		INFO("Making bridge wifi <==> %s:%s\n", opts.getHost(), opts.getPort());
		bridge->makeBridge( opts.interfaceName(), opts.getHost(), atoi(opts.getPort()));
		INFO("Bridge closed. Freeing resources\n");
		delete bridge;
	} else {
		serv->wait();
	}
	if(opts.interfaceName() && strcmp(opts.getHost(), "localhost") == 0) {
		serv->stop();
		INFO("Waiting for server to finish...\n");
#ifdef _WIN32
		Sleep(1000 * 2);
#else
		sleep(2);
#endif
		delete serv;
	}
	return 0;
}
