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
*	Server TCP/UDP skel
*Special Notes:
*	TODO: incomplete
*
*Copyright Stuff:
* 	TODO: put GPLv3 header
*/

#include "Server.h"

Server::Server()
{
	
}

void Server::start() {
	pthread_create(&th, NULL, &run, NULL);
}

void *Server::run(void *arg) {
	while(true) {
		
	}
	return NULL;
}

Server::~Server()
{
}
