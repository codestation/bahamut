/*
 * TCPThread.cpp
 *
 *  Created on: 29/01/2010
 *      Author: code
 */

#include "TCPThread.h"
#include "Logger.h"

bool TCPThread::stop_all = false;

TCPThread::TCPThread(Socket *s) {
	sock = s;
}

int TCPThread::run() {

	char data[512];
	StreamSocket stream(sock);
	char request[128];
	if(stream.openStream()) {
		while(!stop_all) {
			if(stream.readStream(data, 128)) {
				INFO("| %s", data);
				if(!strncmp(data, "GET",3)) {
					strtok(data, " ");
					strcpy(request, strtok(NULL, " "));
				}
				if(!strcmp(data, "\r\n")) {
					if(!strcmp(request,"/")) {
						char message[] = "<h1>Bahamut v0.1 hello world :)</h1>\n";
						INFO("-> Answering 200 OK\n");
						stream.writeStream("HTTP/1.1 200 OK\r\n");
						stream.writeStream("Content-Type: text/html\r\n");
						stream.writeStream("Accept-Ranges: bytes\r\n");
						stream.writeStream("Content-Length: 37\r\n");
						stream.writeStream("Server: bahamut v0.1\r\n");
						stream.writeStream("\r\n");
						INFO("-> Message to browser: %s", message);
						stream.writeStream(message);
					} else {
						char message[] = "404: Not found\n";
						INFO("-> Answering 404 Not Found\n");
						stream.writeStream("HTTP/1.1 404 Not Found\r\n");
						stream.writeStream("Content-Type: text/html\r\n");
						stream.writeStream("Content-Length: 15\r\n");
						stream.writeStream("Server: bahamut v0.1\r\n");
						stream.writeStream("\r\n");
						INFO("-> Message to browser: %s", message);
						stream.writeStream(message);
					}
				}
			} else {
				if(sock->getLastError() == 0) {
					INFO("== TCP Server: End of stream reached\n");
					break;
				}
				if(!sock->readAgain()) {
					ERR("== TCP Server: error occurred while receiving packet\n");
					ERR("== TCP Server: %s (%i)\n", sock->getLastErrorMessage(), sock->getLastError());
				}
			}
		}
		stream.closeStream();
	} else {
		ERR("== TCP Server: cant open stream\n");
	}

	delete sock;
	delete this;
	return 0;
}

TCPThread::~TCPThread() {
}
