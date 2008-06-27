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
}
PspPacket packet;
int capture_func(const u_char *data, u_int len) {
	packet.setPayload(data, len);
	const u_char *mac = packet.getSrcMAC();
	if((mac[0] == 0x0 && mac[1] == 0x11 && mac[2] == 0x22) || (mac[0] == 0 && mac[1] == 0x66 && mac[2] == 0x77))
		return 0;
	else
		return 1;
}

int main(int argc, char ** argv) {
	int n;
	char *dev;
	if(argc != 3) {
		printf("Usage: bahamuth <host> <port>\n");
		return 1;
	}
	printf("Bahamut engine SVN (prototype) starting...\n");

	List *lst = Interface::getAdapterList();
	if(!lst) {
		printf("Cant get the interface list. Exiting...\n");
		return 1;
	}
	int i = 0;
	while(lst->hasNext()) {
		InterfaceInfo *inf = (InterfaceInfo *)lst->next();
		printf("%i) %s [%s]\n", i++, inf->name, inf->desc);
	}
	printf("Select your device by number (0-%i): ", i - 1);
	scanf("%i", &n);
	if(n < 0 || n > (i-1)) {
		printf("\nInvalid selection. Exiting...\n");
		return 1;
	} else {
		dev = strdup(((InterfaceInfo *)lst->getByIndex(n))->name);
	}
	delete lst;

	printf("Intalling signal handler...\n");
	signal(SIGINT, exit_signal);

	if(strcmp(argv[1], "localhost") == 0) {
		printf("Creating UDP server...\n");
		serv = new UDPServer(atoi(argv[2]));
		printf("Starting UDP server...\n");
		serv->start();
	}

	bridge = new DeviceBridge();
	//bridge->registerCaptureCallback(capture_func);
	printf("Making bridge %s <==> %s:%s\n", dev, argv[1], argv[2]);
	bridge->makeBridge( dev, argv[1], atoi(argv[2]));
	printf("Bridge closed. Freeing resources\n");
	free(dev);
	if(strcmp(argv[1], "localhost") == 0) {
		serv->stop();
		delete serv;
	}
	return 0;
}
