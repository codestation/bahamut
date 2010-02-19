/*
 * Parser.h
 *
 *  Created on: 18/02/2010
 *      Author: code
 */

#ifndef PARSER_H_
#define PARSER_H_

#include <ctype.h>
#include "StreamSocket.h"
#include "List.h"

class HTTPParser {

public:
	enum parser_state {PARSE_COMPLETE, PARSE_ERROR, PARSE_INCOMPLETE, PARSE_BUFFER_FULL};

private:
	char buffer[4096];
	List *list;

	unsigned int buffer_pos;
	parser_state parser_status;
	int parse_state;
	int parsed_pos;

	char *uri_addr;
	char *body_addr;
	unsigned int content_length;

	struct data_pair {
		int key_pos;
		char *key_addr;
		char *value_addr;
	};

	int key_index;
	int value_index;
	int content_index;

	enum State {
		p_request_line=0,
		p_request_line_cr=1,
		p_request_line_crlf=2,
		p_request_line_crlfcr=3,
		p_key=4,
		p_key_colon=5,
		p_key_colon_sp=6,
		p_value=7,
		p_value_cr=8,
		p_value_crlf=9,
		p_value_crlfcr=10,
		p_content=11, // here we are done parsing the header.
		p_error=12 // here an error has occurred and the parse failed.
	};

	static int compareFunc(void *, void *);
	static void deleteFunc(void *);

	void parseHeader();

public:

	HTTPParser();
	parser_state addData(const char *data, int size);
	const char *getURI();
	const char *getBody();
	const char *getQuery();
	const char *getValue(const char *key);
	int getContentLength();
	void clear();
	virtual ~HTTPParser();
};

#endif /* PARSER_H_ */
