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

#include "HTTPParser.h"

HTTPParser::HTTPParser() {
	list = new List(compareFunc, deleteFunc);
	buffer_pos = 0;
	parser_status = PARSE_INCOMPLETE;
	key_index = 0;
	value_index = 0;
	content_start = 0;
	parsed_pos = 0;
	content_length = 0;
	parse_state = 0;
	uri_start = 0;
	args_start = 0;
}

HTTPParser::parser_state HTTPParser::addData(const char *data, int size) {
	if(parser_status != PARSE_INCOMPLETE)
		return parser_status;
	if((buffer_pos + size) >= sizeof(buffer)) {
		parser_status = PARSE_BUFFER_FULL;
		return parser_status;
	}
	memcpy(buffer + buffer_pos, data, size);
	buffer_pos += size;
	if(parse_state < p_content)
		parseHeader();
	if(parse_state == p_error) {
		parser_status = PARSE_ERROR;
	} else if(parse_state == p_content) {
		if(content_length == 0 || buffer_pos - content_start >= content_length) {
			if(parseRequestLine()) {
				parser_status = PARSE_COMPLETE;
			} else {
				parser_status = PARSE_ERROR;
			}
		}
	}
	return parser_status;
}

void HTTPParser::parseHeader() {
	// run the fsm.
	const int  CR = 13;
	const int  LF = 10;
	const int  ANY = 256;

	enum Action {
		// make lower case
		LOWER = 0x1,
		// convert current character to null.
		NULLIFY = 0x2,
        // set the header index to the current position
        SET_HEADER_START = 0x4,
        // set the key index to the current position
        SET_KEY = 0x8,
        // set value index to the current position.
        SET_VALUE = 0x10,
        // store current key/value pair.
        STORE_KEY_VALUE = 0x20,
        // sets content start to current position + 1
        SET_CONTENT_START = 0x40
	};

	static const struct FSM {
		State curState;
		int c;
		State nextState;
		unsigned actions;
	} fsm[] = {
		{ p_request_line,         CR, p_request_line_cr,     NULLIFY                            },
		{ p_request_line,        ANY, p_request_line,        0                                  },
		{ p_request_line_cr,      LF, p_request_line_crlf,   0                                  },
		{ p_request_line_crlf,    CR, p_request_line_crlfcr, 0                                  },
		{ p_request_line_crlf,   ANY, p_key,                 SET_HEADER_START | SET_KEY | LOWER },
		{ p_request_line_crlfcr,  LF, p_content,             SET_CONTENT_START                  },
		{ p_key,                 ':', p_key_colon,           NULLIFY                            },
		{ p_key,                 ANY, p_key,                 LOWER                              },
		{ p_key_colon,           ' ', p_key_colon_sp,        0                                  },
		{ p_key_colon_sp,        ANY, p_value,               SET_VALUE                          },
		{ p_value,                CR, p_value_cr,            NULLIFY | STORE_KEY_VALUE          },
		{ p_value,               ANY, p_value,               0                                  },
		{ p_value_cr,             LF, p_value_crlf,          0                                  },
		{ p_value_crlf,           CR, p_value_crlfcr,        0                                  },
		{ p_value_crlf,          ANY, p_key,                 SET_KEY | LOWER                    },
		{ p_value_crlfcr,         LF, p_content,             SET_CONTENT_START                  },
		{ p_error,               ANY, p_error,               0                                  }
	};

	for( unsigned i = parsed_pos; i < buffer_pos; ++i) {
		char c = buffer[i];
		State nextState = p_error;

		for ( unsigned d = 0; d < sizeof(fsm) / sizeof(FSM); ++d ) {
			if ( fsm[d].curState == parse_state &&	( c == fsm[d].c || fsm[d].c == ANY ) ) {

				nextState = fsm[d].nextState;

				if ( fsm[d].actions & LOWER ) {
					buffer[i] = tolower(buffer[i]);
				}

				if ( fsm[d].actions & NULLIFY ) {
					buffer[i] = 0;
				}

				//if ( fsm[d].actions & SET_HEADER_START ) {
				//	_headerStart = i;
				//}

				if ( fsm[d].actions & SET_KEY ) {
					key_index = i;
				}

				if ( fsm[d].actions & SET_VALUE ) {
					value_index = i;
				}

				if ( fsm[d].actions & SET_CONTENT_START ) {
					content_start = i + 1;
				}

				if ( fsm[d].actions & STORE_KEY_VALUE ) {
					data_pair *pair = (data_pair *)malloc(sizeof(data_pair));
					pair->key_pos = i;
					pair->key_addr = buffer + key_index;
					pair->value_addr = buffer + value_index;
					list->add(pair);
				}

				break;
			}
		}
		parse_state = nextState;

		if ( parse_state == p_content ) {
			const char* str = getValue("content-length");
			if ( str ) {
				content_length = atoi(str);
			}
			break;
		}
	}

	parsed_pos = buffer_pos;
}
/*
void HTTPParser::parseArgs(char *args) {
	int len = strlen(args);
	char *key = args;
	char *value = 0;
	do {
		switch(*args) {
		case '=':
			*args = 0;
			value = args + 1;
			break;
		case '&':
			*args = 0;
			data_pair *dat = (data_pair *)malloc(sizeof(data_pair));
			dat->key_pos = key - buffer;
			dat->key_addr = key;
			dat->value_addr = value;
			arg->add(dat);
		}
	} while(args++);
}*/

bool HTTPParser::parseRequestLine()
{
	// find the URI start
	char *str1 = (char *)memchr(buffer, ' ', sizeof(buffer));
	if(!str1)
		return false;
	//find the URI end
	char *str2 = (char *)memchr(str1 + 1, ' ', sizeof(buffer) - (str1 + 1 - buffer));
	if(!str2)
		return false;
	str1[0] = 0;
	str2[0] = 0;
	uri_start = str1 - buffer + 1;
	// find the arguments start (if any)
	str1 = (char *)memchr(buffer + uri_start, '?', str2 - str1);
	if(str1)
		args_start = (str1 + 1) - buffer;
    return true;
}

HTTPParser::parser_state HTTPParser::getState() {
	return parser_status;
}

const char *HTTPParser::getArgs() {
	if(args_start > 0)
		return buffer + args_start;
	return 0;
}

const char *HTTPParser::getURI() {
	return (const char *)buffer + uri_start;
}

const char *HTTPParser::getMethod() {
	return buffer;
}

const char *HTTPParser::getValue(const char *key) {
	data_pair *pair = (data_pair *)list->get((void *)key);
	return pair ? pair->value_addr : 0;
}

int HTTPParser::compareFunc(void *obj, void *item) {
	unsigned int key_pos = ((data_pair *)obj)->key_pos;
	//unsigned int key_pos = ((data_pair *)obj)->key_addr - buffer;
	unsigned int key_len = strlen((const char *)item);
	if((key_pos + key_len) < sizeof(buffer))
		return strncmp(((data_pair *)obj)->key_addr, (const char *)item, key_len);
	return 1;
}

void HTTPParser::deleteFunc(void *obj) {
	free((data_pair *)obj);
}

void HTTPParser::clear() {
	list->clear();
	args_start = 0;
	buffer_pos = 0;
	parser_status = PARSE_INCOMPLETE;
}


HTTPParser::~HTTPParser() {
	delete list;
}
