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
