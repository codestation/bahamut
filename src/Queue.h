/*
 * Queue.h
 *
 *  Created on: 24/02/2010
 *      Author: code
 */

#ifndef QUEUE_H_
#define QUEUE_H_

class Queue {
	typedef void (*DELETE_FUNC) (void *);
	DELETE_FUNC del;
	struct node {
		void *data;
		node *prev;
	} *head, *tail;
public:
	Queue(DELETE_FUNC);
	void put(void *obj);
	void *pop();
	bool empty();
	void clear();
	virtual ~Queue();
};

#endif /* QUEUE_H_ */
