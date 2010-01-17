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
 *     Startup file
 * Special Notes:
 *     TODO: more cleanup, remove all the possible code and make some API
 *     		 to communicate with the UI
 */

#ifdef _WIN32
#include <windows.h>
#endif
#include <stdlib.h>
#include <signal.h>
#include "Interface.h"
#include "UDPServer.h"
#include "DeviceBridge.h"

UDPServer *serv = NULL;
DeviceBridge *bridge = NULL;

void exit_signal(int signal) {
	printf("\nTrapped CTRL+C signal, shutting down...\n");
	if(bridge) {
		bridge->removeBridge();
	}
	serv->stop();
}
/*
int capture_func(const u_char *mac, u_int len) {
	u_int manufacturer = (mac[6] << 16) | (mac[7] << 8) | mac[8];
	return (manufacturer == 0x001C26 || manufacturer == 0x001DD9 || manufacturer == 0x0016FE) ? 0 : 1;
	//return (manufacturer == 0x001C26 || manufacturer == 0x001DD9 || manufacturer == 0x00197D) ? 0 : 1;
}
*/
/*
int inject_func(PspPacket *packet) {
	total_received++;
	return 0;
}*/



int main(int argc, char ** argv) {
	int n;
	bool order = true;
	char *dev;
	if(argc < 3) {
		printf("Usage: bahamuth <host> <port> (disableorder)\n");
		return 1;
	}
	if(argc == 4 && !strcmp(argv[3], "disableorder")) {
		order = false;
		printf("Packet ordering disabled.\n");
	}
	printf("Bahamut engine SVN r26 starting...\n");

	List *lst = Interface::getAdapterList();
	if(!lst) {
		printf("Cant get the interface list. Exiting...\n");
		return 1;
	}
	int i = 0;
	while(lst->hasNext()) {
		InterfaceInfo *inf = (InterfaceInfo *)lst->next();
#ifdef _WIN32
		printf("%i) %s\n", i++, inf->desc);
#else
		printf("%i) %s\n", i++, inf->name);
#endif
	}
	printf("%i) Dedicated server (no bridge)\n", i++);
	printf("Select your device by number (0-%i): ", i - 1);
	int val = scanf("%i", &n);
	if(val != 1 || n < 0 || n > (i-1)) {
		printf("\nInvalid selection. Exiting...\n");
		return 1;
	} else {
		if(n == (i-1))
			dev = NULL;
		else
			dev = strdup(((InterfaceInfo *)lst->getByIndex(n))->name);
	}
	delete lst;

	printf("Intalling signal handler...\n");
	signal(SIGINT, exit_signal);

	if(strcmp(argv[1], "localhost") == 0) {
		printf("Creating UDP server...\n");
		serv = new UDPServer(atoi(argv[2]), order);
		printf("Starting UDP server...\n");
		serv->start();
	}
	if(dev) {
		bridge = new DeviceBridge(order);
		//bridge->registerCaptureCallback(capture_func);
		//bridge->registerInjectCallback(inject_func);
		printf("Making bridge wifi <==> %s:%s\n", argv[1], argv[2]);
		bridge->makeBridge( dev, argv[1], atoi(argv[2]));
		printf("Bridge closed. Freeing resources\n");
		delete bridge;
	} else {
		serv->wait();
	}
	if(dev && strcmp(argv[1], "localhost") == 0) {
		free(dev);
		serv->stop();
		printf("Waiting for server to finish...\n");
#ifdef _WIN32
		Sleep(1000 * 2);
#else
		sleep(2);
#endif
		delete serv;
	}
	return 0;
}
