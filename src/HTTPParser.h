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

#ifndef PARSER_H_
#define PARSER_H_

#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include "core/List.h"

#define BUFFER_SIZE 4096

class HTTPParser {

public:
	enum parser_state {PARSE_COMPLETE,PARSE_INCOMPLETE, PARSE_ERROR, PARSE_BUFFER_FULL};

private:
	char buffer[BUFFER_SIZE];
	List *list;

	unsigned int buffer_pos;
	parser_state parser_status;
	int parse_state;
	int parsed_pos;

	unsigned int content_length;

	struct data_pair {
		int key_pos;
		char *key_addr;
		char *value_addr;
	};


	int key_index;
	int value_index;

	int uri_start;
	int args_start;
	int content_start;

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
	int getContentLength();
	bool parseRequestLine();

public:
	HTTPParser();
	parser_state addData(const char *data, int size);
	const char *getURI();
	const char *getArgs();
	const char *getBody();
	const char *getQuery();
	const char *getMethod();
	parser_state getState();
	const char *getValue(const char *key);
	void clear();
	virtual ~HTTPParser();
};

#endif /* PARSER_H_ */
