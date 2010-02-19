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

void TCPThread::sendHTTPHeader(int code, const char *message, long int content_size) {
	char buffer[512];
	int len;
	char response[] = "HTTP/1.1 %i %s\r\n";
	char type[] = "Content-Type: %s\r\n";
	char length[] = "Content-Length: %i\r\n";
	char server[] = "Server: %s\r\n";
	char header_end[] = "\r\n";
	len = sprintf(buffer, response, code, message);
	sock->writeSocket(buffer, len);
	len = sprintf(buffer, type, "text/html");
	sock->writeSocket(buffer, len);
	len = sprintf(buffer, length, content_size);
	sock->writeSocket(buffer, len);
	len = sprintf(buffer, server, "Bahamut v0.1");
	sock->writeSocket(buffer, len);
	sock->writeSocket(header_end, sizeof(header_end));
}

int TCPThread::sendFile(const char *uri) {
	char buffer[512];
	char clean_uri[256];
	int end_uri = strchr(uri, ' ') - uri;
	strncpy(clean_uri, uri, end_uri);
	clean_uri[end_uri] = '\0';
	if(strlen(clean_uri) > 256)
		return -1;
	if(clean_uri[strlen(clean_uri)-1] == '/')
		strcat(clean_uri, "index.html");
	sprintf(buffer, "%s%s", WWWROOT, clean_uri);

	struct stat statbuf;
	if (stat(buffer, &statbuf) == -1) {
		return -1;
	} else {
		if(S_ISDIR(statbuf.st_mode))
			return -1;
	}

	FILE *fd = fopen(buffer, "r");
	if(fd) {
		fseek(fd, 0, SEEK_END);
		long int size = ftell(fd);
		fseek(fd, 0, SEEK_SET);
		sendHTTPHeader(200,"OK", size);
		while(!feof(fd)) {
			int size = fread(buffer, 1, sizeof(buffer) ,fd);
			sock->writeSocket(buffer, size);
		}
		fclose(fd);
		return 0;
	} else {
		sendHTTPHeader(404, "Not found", 29);
		sock->writeSocket("<h1>Error 404: Not found</h1>", 29);
	}
	return -1;
}

int TCPThread::run() {
	char data[512];
	HTTPParser parser;
	while(!stop_all) {
		int read = sock->readSocket(data, sizeof(data));
		if(read > 0) {
			int result = parser.addData(data, read);
			if(result == HTTPParser::PARSE_COMPLETE) {
				if(sendFile(parser.getURI()) < 0) {
					sendHTTPHeader(500, "Internal server error", 41);
					sock->writeSocket("<h1>Error 500: Internal server error</h1>", 41);
				}
				parser.clear();
			} else if(result == HTTPParser::PARSE_ERROR || result == HTTPParser::PARSE_BUFFER_FULL) {
				sendHTTPHeader(500, "Internal server error", 41);
				sock->writeSocket("<h1>Error 500: Internal server error</h1>", 41);
				parser.clear();
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
