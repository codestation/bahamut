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
