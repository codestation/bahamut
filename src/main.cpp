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
#include <stdio.h>
#include <signal.h>
#include "UDPServer.h"
#include "TCPServer.h"
#include "DeviceBridge.h"
#include "Logger.h"
#include "GetOpt.h"
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

void usage() {
	INFO("bahamut-engine SVN r%s.\n\n", SVN_REV);
	INFO("Usage: bahamuth-engine -h <host> -p <port>  [-i <interface>] [-l] [-o] [-d] [-v]\n\n");
	INFO("-h    Connect to host (use localhost to create a server)\n");
	INFO("-p    Local/remote port number\n");
	INFO("-i    Network device to use\n");
	INFO("-l    List network devices and exit\n");
	INFO("-o    Disable packet ordering rule\n");
	INFO("-d    Create dedicated server (dont open device)\n");
	INFO("-v    Show verbose output (twice for debug output)\n");
}

int main(int argc, char ** argv) {
	char option = 0;
	int disable_order, verbose = 0;
	bool list_interfaces = false;
	bool dedicated = false;
	bool empty = true;
	const char *interface = NULL;
	const char *host = NULL;
	const char *port = NULL;

	GetOpt opt(argc, argv, "h:p:i:vold");
	while(option != '?' && (option = opt()) != EOF) {
		switch(option) {
		case 'v': verbose++; break;
		case 'o': disable_order = 1; break;
		case 'l': list_interfaces = true; break;
		case 'd': dedicated = true; break;
		case 'i': interface = opt.arg(); break;
		case 'h': host = opt.arg(); break;
		case 'p': port = opt.arg(); break;
		case '?':
			INFO("\n");
			usage();
		default:
			return EXIT_FAILURE;
		}
		empty = false;
	}
	INFO_ON();
	if(empty) {
		usage();
		return EXIT_FAILURE;
	}
	if(list_interfaces) {
		List *lst = Interface::getAdapterList();
		if(!lst) {
			ERR("Cant get the interface list. Exiting...\n");
			return EXIT_FAILURE;
		}
		int i = 0;
		INFO("Interface list:\n");
		while(lst->hasNext()) {
			InterfaceInfo *inf = (InterfaceInfo *)lst->next();
			INFO("(%i) %s - %s\n", i++, inf->name, inf->desc);
		}
		delete lst;
		return EXIT_SUCCESS + 2;
	}

	if(!verbose)
		INFO_OFF();
	if(verbose > 1)
		DEBUG_ON();

	INFO("Bahamut engine SVN r%s starting...\n", SVN_REV);

	if(!host) {
		ERR("No host defined, exiting...\n");
		return EXIT_FAILURE;
	}

	if(!port) {
		ERR("No port defined, exiting...\n");
		return EXIT_FAILURE;
	}

	if(!interface && !dedicated) {
		ERR("No interface defined, exiting...\n");
		return EXIT_FAILURE;
	}

	INFO("Installing signal handler...\n");
	signal(SIGINT, exit_signal);

	if(strcmp(host, "localhost") == 0) {
		udp_serv = new UDPServer(atoi(port), !disable_order);
		INFO("Starting UDP server...\n");
		udp_serv->start();
		//tcp_serv = new TCPServer(atoi(opts.getPort()));
		//INFO("Starting TCP server...\n");
		//tcp_serv->start();
	}
	if(!dedicated) {
		if(udp_serv)
			udp_serv->detach();
		if(tcp_serv)
			tcp_serv->detach();
		bridge = new DeviceBridge(!disable_order);
		INFO("Making bridge wifi <==> %s:%s\n", host, port);
		if(!bridge->makeBridge( interface, host, atoi(port)))
			ERR("Error while making bridge\n");
		INFO("Bridge closed. Freeing resources\n");
		delete bridge;
	} else {
		if(udp_serv)
			udp_serv->wait();
		if(tcp_serv)
			tcp_serv->wait();
	}
	if(interface && strcmp(host, "localhost") == 0) {
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
	return EXIT_SUCCESS;
}
