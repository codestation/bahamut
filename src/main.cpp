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

#ifdef _WIN32
#define WINVER 0x0501 //Windows XP
#include <windows.h>
#endif
#include <signal.h>
#include "UDPServer.h"
#include "TCPServer.h"
#include "DeviceBridge.h"
#include "Logger.h"
#include "ArgParser.h"
#include "TCPThread.h"

UDPServer *udp_serv = NULL;
TCPServer *tcp_serv = NULL;
DeviceBridge *bridge = NULL;

void exit_signal(int signal) {
	INFO("\nTrapped CTRL+C signal, shutting down...\n");
	if(bridge)
		bridge->removeBridge();
	if(udp_serv)
		udp_serv->stop();
	if(tcp_serv)
		tcp_serv->stop();
}

int main(int argc, char ** argv) {
	ArgParser opts;
	INFO_ON();
	if(!opts.parse(argc, argv)) {
		INFO("bahamut-engine SVN r%s.\n\n", SVN_REV);
		INFO("Usage: bahamuth-engine -h <host> -p <port>  [-i <interface>] [-l] [-o] [-d] [-v]\n\n");
		INFO("-h    Connect to host (use localhost to create a server)\n");
		INFO("-p    Local/remote port number\n");
		INFO("-i    Network device to use\n");
		INFO("-l    List network devices and exit\n");
		INFO("-o    Disable packet ordering rule\n");
		INFO("-d    Create dedicated server (dont open device)\n");
		INFO("-v    Show verbose output (twice for debug output)\n");
		return 1;
	}
	if(opts.listInterfaces()) {
		List *lst = Interface::getAdapterList();
		if(!lst) {
			ERR("Cant get the interface list. Exiting...\n");
			return 1;
		}
		int i = 0;
		INFO("Interface list:\n");
		while(lst->hasNext()) {
			InterfaceInfo *inf = (InterfaceInfo *)lst->next();
			INFO("(%i) %s - %s\n", i++, inf->name, inf->desc);
		}
		delete lst;
		return 2;
	}

	if(!opts.verboseMode())
		INFO_OFF();
	if(opts.verboseMode() > 1)
		DEBUG_ON();

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

	INFO("Installing signal handler...\n");
	signal(SIGINT, exit_signal);

	if(strcmp(opts.getHost(), "localhost") == 0) {
		udp_serv = new UDPServer(atoi(opts.getPort()), !opts.disableOrdering());
		INFO("Starting UDP server...\n");
		udp_serv->start();
		//tcp_serv = new TCPServer(atoi(opts.getPort()));
		//INFO("Starting TCP server...\n");
		//tcp_serv->start();
	}
	if(!opts.dedicatedServer()) {
		if(udp_serv)
			udp_serv->detach();
		if(tcp_serv)
			tcp_serv->detach();
		bridge = new DeviceBridge(!opts.disableOrdering());
		INFO("Making bridge wifi <==> %s:%s\n", opts.getHost(), opts.getPort());
		if(!bridge->makeBridge( opts.interfaceName(), opts.getHost(), atoi(opts.getPort())))
			ERR("Error while making bridge\n");
		INFO("Bridge closed. Freeing resources\n");
		delete bridge;
	} else {
		if(udp_serv)
			udp_serv->wait();
		if(tcp_serv)
			tcp_serv->wait();
	}
	if(opts.interfaceName() && strcmp(opts.getHost(), "localhost") == 0) {
		if(udp_serv)
			udp_serv->stop();
		if(tcp_serv)
			tcp_serv->stop();
		INFO("Waiting for server to finish...\n");
#ifdef _WIN32
		Sleep(1000 * 2);
#else
		sleep(2);
#endif
	}
	delete udp_serv;
	delete tcp_serv;
	return 0;
}
