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
#include "core/Logger.h"

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
			switch(result) {
			case HTTPParser::PARSE_COMPLETE:
				response.generateResponse(&parser, sock);
				break;
			case HTTPParser::PARSE_INCOMPLETE:
				//DEBUG("Received %i bytes of data so far\n", read, sock->getDescriptor());
				continue;
			case HTTPParser::PARSE_BUFFER_FULL:
				response.sendError(sock, 414, HTTPResponse::error_414_reason, true);
				break;
			case HTTPParser::PARSE_ERROR:
				response.sendError(sock, 400, HTTPResponse::error_400_reason, true);
				break;
			default:
				response.sendError(sock, 500, HTTPResponse::error_500_reason);
			}
			parser.clear();
		} else {
			if(read < 0) {
				if(sock->getLastError() == 0) {
					INFO("== TCPThread: End of stream reached\n");
					break;
				}
				if(!sock->readAgain()) {
					ERR("error occurred while receiving packet: %s\n", sock->getLastErrorMessage());
				}
			}
		}
	}
	sock->shutdownSocket();
	INFO("Connection closed with %s:%i\n", sock->getIpAddress(), sock->getPort());
	delete sock;
	delete this;
	return 0;
}

TCPThread::~TCPThread() {
}
