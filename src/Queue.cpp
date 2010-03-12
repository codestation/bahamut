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
	pthread_mutex_init(&mutex, 0);
}

void Queue::put(void *obj) {
	pthread_mutex_lock(&mutex);
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
	pthread_mutex_unlock(&mutex);
}

void *Queue::pop() {
	pthread_mutex_lock(&mutex);
	node *ret = tail;
	if(head == tail)
		head = 0;
	tail = tail->prev;
	ret->prev = 0;
	pthread_mutex_unlock(&mutex);
	return ret;
}

bool Queue::empty() {
	return head != 0;
}

void Queue::clear() {
	pthread_mutex_lock(&mutex);
	node *tmp = tail;
	while(tmp) {
		tmp = tmp->prev;
		if(del)
			del(tail);
		delete tail;
		tail = tmp;
	}
	head = tail = 0;
	pthread_mutex_unlock(&mutex);
}

Queue::~Queue() {
	clear();
	pthread_mutex_destroy(&mutex);
}
