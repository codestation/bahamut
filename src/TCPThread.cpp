/*
 * TCPThread.cpp
 *
 *  Created on: 29/01/2010
 *      Author: code
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include "TCPThread.h"
#include "Logger.h"

bool TCPThread::stop_all = false;

TCPThread::TCPThread(Socket *s) {
	sock = s;
}

int TCPThread::run() {
	char data[512];
	HTTPParser parser;
	while(!stop_all) {
		int read = sock->readSocket(data, sizeof(data));
		if(read > 0) {
			int result = parser.addData(data, read);
			if(result == HTTPParser::PARSE_COMPLETE) {
				const char *uri = parser.getURI();
				if(uri) {
					if(!strcmp(uri,"/query")) {
						char buffer[128];
						strcpy(buffer, response.getDate());
						response.sendData(sock, buffer, strlen(buffer));
					} else {
						response.sendFile(sock, uri);
					}
				} else {
					response.sendError(sock, 414, "Request-URI Too Long");
				}
				parser.clear();
			} else {
				if(result == HTTPParser::PARSE_ERROR || result == HTTPParser::PARSE_BUFFER_FULL) {
					response.sendError(sock, 500, "Internal server error");
					parser.clear();
				}
			}
		} else {
			if(read < 0) {
				if(sock->getLastError() == 0) {
					INFO("== TCPThread: End of stream reached\n");
					break;
				}
				if(!sock->readAgain()) {
					ERR("== TCPThread: error occurred while receiving packet\n");
					ERR("== TCPThread: %s (%i)\n", sock->getLastErrorMessage(), sock->getLastError());
				}
			}
		}
	}
	delete sock;
	delete this;
	return 0;
}

TCPThread::~TCPThread() {
}
