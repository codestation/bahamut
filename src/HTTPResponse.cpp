/*
 * HTTPHeader.cpp
 *
 *  Created on: 18/02/2010
 *      Author: code
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
const char *HTTPResponse::etag = "ETag: %s\r\n";
const char *HTTPResponse::empty = "\r\n";

const char *HTTPResponse::mime_application_stream = "application/octet-stream";
const char *HTTPResponse::mime_text_html = "text/html";
const char *HTTPResponse::mime_text_javascript = "text/javascript";
const char *HTTPResponse::mime_text_plain = "text/plain";

const char *HTTPResponse::error_template = "<h1>Error %i: %s</h1>";

HTTPResponse::HTTPResponse() {
	buffer_pos = 0;
}

const char *HTTPResponse::getDate(time_t *tv) {
	tm local;
	time_t t = time(tv);
#ifdef _WIN32
	_gmtime_s(&t, &local);
	asctime_s(date_buffer, sizeof(date_buffer),local);
#else
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
	return s->writeSocket(buffer, buffer_pos);
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
		return NULL;
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
				s->writeSocket(buffer, size);
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
	s->writeSocket(builtin_msg, size);
	clear();
	return 1;
}

int HTTPResponse::sendFile(Socket *s, const char *uri) {
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
			fseek(fd, 0, SEEK_SET);
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
				s->writeSocket(buffer, size);
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
	s->writeSocket(data, size);
	clear();
	return 0;
}

HTTPResponse::~HTTPResponse() {
	// TODO Auto-generated destructor stub
}
