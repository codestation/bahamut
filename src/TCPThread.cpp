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
		int read = sock->receiveData(data, sizeof(data));
		if(read > 0) {
			int result = parser.addData(data, read);
			if(result == HTTPParser::PARSE_COMPLETE) {
				const char *uri = parser.getURI();
				if(strlen(uri) > 127)
					response.sendError(sock, 500, "Internal server error");
				else {
					if(!strcmp(uri,"/query")) {
						char buffer[128];
						strcpy(buffer, response.getDate());
						response.sendData(sock, buffer, strlen(buffer));
					} else {

						response.sendFile(sock, uri, parser.getValue("Range"));
					}
				}
				parser.clear();
			} else {
				if(result == HTTPParser::PARSE_ERROR) {
					response.sendError(sock, 400, "Bad Request", true);
					parser.clear();
					break;
				} else if(result == HTTPParser::PARSE_BUFFER_FULL) {
					response.sendError(sock, 414, "Request-URI Too Long", true);
					parser.clear();
					break;
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
	sock->shutdownSocket();
	INFO("== TCPThread: Connection closed with %s:%i\n", sock->getIpAddress(), sock->getPort());
	delete sock;
	delete this;
	return 0;
}

TCPThread::~TCPThread() {
}
