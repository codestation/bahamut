/*
 * QueryBuilder.cpp
 *
 *  Created on: 24/02/2010
 *      Author: code
 */

#include "QueryBuilder.h"

QueryBuilder::QueryBuilder() {
	buffer_pos = 0;
}

const char *QueryBuilder::solveQuery(const char *args) {
	char *key = strdup(args);
	char *value = strchr(key, '=');
	if(!value)
		return 0;
	*value++ = 0;
	if(strchr(value, '&'))
		return 0;
	if(!strcmp(key, "statistics")) {
		if(!strcmp(value, "packets")) {

		} else if(!strcmp(value, "download")) {

		} else if(!strcmp(value, "upload")) {

		} else if(!strcmp(value, "packets")) {

		}
	} else if(!strcmp(key, "list")) {
		if(!strcmp(value, "clients")) {

		} else if(!strcmp(value, "localDevices")) {

		} else if(!strcmp(value, "remoteDevices")) {

		}
	} else if(!strcmp(key, "infoclient")) {

	} else if(!strcmp(key, "infodevice")) {
	}
	free(key);
	return buffer;
}

QueryBuilder::~QueryBuilder() {
	// TODO Auto-generated destructor stub
}
