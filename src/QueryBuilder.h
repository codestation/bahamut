/*
 * QueryBuilder.h
 *
 *  Created on: 24/02/2010
 *      Author: code
 */

#ifndef QUERYBUILDER_H_
#define QUERYBUILDER_H_

#include <string.h>
#include <stdlib.h>
#include "Queue.h"
#include "List.h"

class QueryBuilder {
	Queue *queue;
	List *clients;
	List *local_devices;
	List *remote_devices;
	unsigned long last_received;
	char buffer[4096];
	int buffer_pos;
public:
	QueryBuilder();
	void appendData(void *data);
	const char *solveQuery(const char *args);
	void clear() { buffer_pos = 0; }
	virtual ~QueryBuilder();
};

#endif /* QUERYBUILDER_H_ */
