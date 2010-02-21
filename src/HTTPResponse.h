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

#ifndef HTTPRESPONSE_H_
#define HTTPRESPONSE_H_

#include <time.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "Socket.h"

#define WWWROOT "/home/code/public"

class HTTPResponse {
public:
	static const char *response;
	static const char *date;
	static const char *type;
	static const char *length;
	static const char *server;
	static const char *modified;
	static const char *etag;
	static const char *connection;
	static const char *empty;

	static const char *mime_text_html;
	static const char *mime_text_javascript;
	static const char *mime_text_plain;
	static const char *mime_application_stream;

	static const char *error_template;
/*
	enum header_type {
		HEADER_RESPONSE,
		HEADER_DATE,
		HEADER_TYPE,
		HEADER_LENGTH,
		HEADER_SERVER,
		HEADER_MODIFIED,
		HEADER_EMPTY
	};
*/
private:
	char buffer[1024];
	char header_buffer[256];
	char date_buffer[64];
	unsigned int buffer_pos;

public:
	HTTPResponse();
	bool pushHeader(const char *format, ...);
	int sendHeader(Socket *s);
	int sendFile(Socket *s, const char *uri);
	int sendData(Socket *s, const char *data, int size, const char *mime_type = mime_text_plain);
	int sendError(Socket *s, int code, const char *reason, bool close = false);
	const char *getDate(time_t *tv = NULL);
	int isDirectory(const char *uri);
	time_t lastModified(const char *uri);
	const char *getMime(const char *uri);
	char *mketag(char *s, struct stat *sb);
	void clear() { buffer_pos = 0; }
	virtual ~HTTPResponse();
};

#endif /* HTTPRESPONSE_H_ */
