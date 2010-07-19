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

#include <stdarg.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include <string.h>
#include "HTTPResponse.h"

const char *HTTPResponse::response = "HTTP/1.1 %i %s\r\n";
const char *HTTPResponse::type = "Content-Type: %s\r\n";
const char *HTTPResponse::length = "Content-Length: %i\r\n";
const char *HTTPResponse::server = "Server: %s\r\n";
const char *HTTPResponse::date = "Date: %s\r\n";
const char *HTTPResponse::connection = "Connection: %s\r\n";
const char *HTTPResponse::modified = "Last-Modified: %s\r\n";
//FIXME: use the ETag field
const char *HTTPResponse::etag = "ETag: %s\r\n";
const char *HTTPResponse::empty = "\r\n";

const char *HTTPResponse::mime_application_stream = "application/octet-stream";
const char *HTTPResponse::mime_text_html = "text/html";
const char *HTTPResponse::mime_text_javascript = "text/javascript";
const char *HTTPResponse::mime_text_plain = "text/plain";

const char *HTTPResponse::error_template = "<h1>Error %i: %s</h1>";

const char *HTTPResponse::error_400_reason = "Bad Request";
//FIXME: implement 403
const char *HTTPResponse::error_404_reason = "Not Found";
const char *HTTPResponse::error_414_reason = "Request-URI Too Long";
const char *HTTPResponse::error_500_reason = "Internal Server Error";
const char *HTTPResponse::error_501_reason = "Not Implemented";


HTTPResponse::HTTPResponse() {
	buffer_pos = 0;
}

int HTTPResponse::generateResponse(HTTPParser *parser, Socket *s) {
	if(strcmp(parser->getMethod(), "GET")) {
		sendError(s, 501, HTTPResponse::error_501_reason, true);
		return 1;
	}
	if(strlen(parser->getURI()) >= MAX_URI_SIZE) {
		sendError(s, 500, HTTPResponse::error_500_reason, true);
		return 1;
	} else {
		if(!strcmp(parser->getURI(),"/query")) {
			//TODO: real query support
			char buffer[128];
			strcpy(buffer, getDate());
			return sendData(s, buffer, strlen(buffer));
		} else {
			return sendFile(s, parser->getURI(), parser->getValue("Range"));
		}
	}
	return 0;
}

const char *HTTPResponse::getDate(time_t *tv) {
#ifdef _WIN32
	//_gmtime(&t, &local);
	//asctime(date_buffer, sizeof(date_buffer),local);
	//FIXME not implemented
	strcpy(date_buffer, "Sun Feb 21 20:57:57 2010\n");
#else
	tm local;
	time_t t = time(tv);
	gmtime_r(&t, &local);
	asctime_r(&local, date_buffer);
#endif
	date_buffer[strlen(date_buffer) -1 ] = '\0';
	return date_buffer;
}

char *HTTPResponse::mketag(char *s, struct stat *sb)
{
    sprintf(s, "%d/%d/%d", (int)sb->st_ino, (int)sb->st_mtime, (int)sb->st_size);
    return s;
}

bool HTTPResponse::pushHeader(const char *format, ...) {
	bool ret = false;
	va_list args;
	va_start(args, format);
	unsigned int len = vsprintf(header_buffer, format, args);
	if(buffer_pos + len < sizeof(buffer)) {
		strcpy(buffer + buffer_pos, header_buffer);
		buffer_pos += len;
		ret = true;
	}
	va_end(args);
	return ret;
}

int HTTPResponse::sendHeader(Socket *s) {
	return s->sendData(buffer, buffer_pos);
}

int HTTPResponse::isDirectory(const char *uri) {
	struct stat statbuf;
	if (stat(uri, &statbuf) == -1) {
		return -1;
	} else {
		if(S_ISDIR(statbuf.st_mode))
			return 1;
	}
	return 0;
}

time_t HTTPResponse::lastModified(const char *uri) {
	struct stat statbuf;
	if (stat(uri, &statbuf) == -1)
		return 0;
	return statbuf.st_mtime;
}

const char *HTTPResponse::getMime(const char *uri) {
	const char *ptr = strrchr(uri, '.');
	if(ptr) {
		if(!strcasecmp(ptr, ".html"))
			return mime_text_html;
		if(!strcasecmp(ptr, ".js"))
			return mime_text_javascript;
	}
	return mime_application_stream;
}

int HTTPResponse::sendError(Socket *s, int code, const char *reason, bool close) {
	pushHeader(response, code, reason);
	pushHeader(type, mime_text_html);
	if(close)
		pushHeader(connection, "close");
	pushHeader(server, "Bahamut v0.2");
	pushHeader(date, getDate());

	sprintf(header_buffer, "%s/msg/%i.html", WWWROOT, code);
	if(!isDirectory(header_buffer)) {
		FILE *fd = fopen(header_buffer, "r");
		if(fd) {
			fseek(fd, 0, SEEK_END);
			long int size = ftell(fd);
			fseek(fd, 0, SEEK_SET);

			pushHeader(length, size);
			pushHeader(empty);
			sendHeader(s);

			while(!feof(fd)) {
				int size = fread(buffer, 1, sizeof(buffer) ,fd);
				s->sendData(buffer, size);
			}
			fclose(fd);
			clear();
			return 0;
		}
	}
	char builtin_msg[128];
	int size = sprintf(builtin_msg, error_template, code, reason);
	pushHeader(length, size);
	pushHeader(empty);
	sendHeader(s);
	s->sendData(builtin_msg, size);
	clear();
	return 1;
}

int HTTPResponse::sendFile(Socket *s, const char *uri, const char *range) {
	char tmp_buffer[256];
	if(uri[strlen(uri)-1] == '/')
		sprintf(tmp_buffer, "%s%sindex.html", WWWROOT, uri);
	else
		sprintf(tmp_buffer, "%s%s", WWWROOT, uri);
	if(!isDirectory(tmp_buffer)) {
		FILE *fd = fopen(tmp_buffer, "r");
		if(fd) {
			fseek(fd, 0, SEEK_END);
			long int size = ftell(fd);
			long int start = 0;
			long int end = -1;
			if(!range) {
				fseek(fd, 0, SEEK_SET);
			} else {
				if(range[0] == '-') {
					end = size;
					start = end - atol(range + 1);
					if(start < 0) {
						sendError(s, 400, "Bad Request", true);
						clear();
					}
				} else {
					char *tmpbuf = strdup(range);
					char *mid = strchr(tmpbuf, '-');
					//if(strlen(tmpbuf) == (mid - tmpbuf)) {

					//}
					mid[0] = 0;
					start = atol(tmpbuf);
					free(tmpbuf);

				}
			}
			pushHeader(response, 200, "OK");
			pushHeader(type, getMime(tmp_buffer));
			pushHeader(length, size);
			time_t mod = lastModified(tmp_buffer);
			if(mod)
				pushHeader(modified, getDate(&mod));
			pushHeader(server, "Bahamut v0.3");
			pushHeader(date, getDate());
			pushHeader(empty);
			sendHeader(s);
			while(!feof(fd)) {
				int size = fread(buffer, 1, sizeof(buffer) ,fd);
				s->sendData(buffer, size);
			}
			fclose(fd);
			clear();
			return 0;
		}
	}
	sendError(s, 404, "Not found");
	clear();
	return 1;
}

int HTTPResponse::sendData(Socket *s, const char *data, int size, const char *mime_type) {
	pushHeader(response, 200, "OK");
	pushHeader(type, mime_type);
	pushHeader(length, size);
	pushHeader(server, "Bahamut v0.3");
	pushHeader(date, getDate());
	pushHeader(empty);
	sendHeader(s);
	s->sendData(data, size);
	clear();
	return 0;
}

HTTPResponse::~HTTPResponse() {
	// TODO Auto-generated destructor stub
}
