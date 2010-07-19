/*
 * Queue.cpp
 *
 *  Created on: 24/02/2010
 *      Author: code
 */

#include "Queue.h"

Queue::Queue(DELETE_FUNC dfunc) {
	head = tail = 0;
	del = dfunc;
}

void Queue::put(void *obj) {
	if(!head) {
		head = new node;
		head->data = obj;
		head->prev = 0;
		tail = head;
	} else {
		head->prev = new node;
		head->prev->data = obj;
		head->prev->prev = 0;
		head = head->prev;
	}
}

void *Queue::pop() {
	node *ret = tail;
	if(head == tail)
		head = 0;
	tail = tail->prev;
	ret->prev = 0;
	return ret;
}

bool Queue::empty() {
	return head != 0;
}

void Queue::clear() {
	node *tmp = tail;
	while(tmp) {
		tmp = tmp->prev;
		if(del)
			del(tail);
		delete tail;
		tail = tmp;
	}
	head = tail = 0;
}

Queue::~Queue() {
	clear();
}
