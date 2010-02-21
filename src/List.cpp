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

#include "List.h"

List::List(COMPARE_FUNC cfunc, DELETE_FUNC dfunc) {
	head = 0;
	iter = 0;
	counter = 0;
	comp = cfunc;
	del = dfunc;
}

void List::add(void *obj) {
	if(!head) {
		head = new node;
		head->obj = obj;
		head->next = 0;
	} else {
		node *curr = head;
		while(curr->next)
			curr = curr->next;
		curr->next = new node;
		curr->next->obj = obj;
		curr->next->next = 0;
	}
	iter = head;
	counter++;
}

void *List::get(void *obj) {
	if(head) {
		node *curr = head;
		while(curr) {
			if(!comp(curr->obj, obj))
				return curr->obj;
			curr = curr->next;
		}
	}
	return 0;
}

void *List::getByIndex(int n) {
	node *curr = head;
	while(n-- && curr)
		curr = curr->next;
	return curr ? curr->obj : 0;
}

bool List::exist(void *obj) {
	return get(obj) != 0;
}

bool List::remove(void *obj) {
	if(head) {
		node *curr = head;
		node *prev = head;
		while(curr) {
			if(!comp(curr->obj, obj)) {
				prev->next = curr->next;
				if(head == curr)
					head = curr->next;
				del(curr->obj);
				delete curr;
				iter = head;
				counter--;
				return true;
			}
			prev = curr;
			curr = curr->next;
		}
	}
	return false;
}

int List::count() {
	/*node *curr = head;
	int c = 0;
	while(curr) {
		curr = curr->next;
		c++;
	}
	return c;*/
	return counter;
}


void List::clear() {
	while(head) {
		node *next = head->next;
		del(head->obj);
		delete head;
		head = next;
	}
	iter = head;
	counter = 0;
}

void List::rewind() {
	iter = head;
}

bool List::empty() {
	return counter == 0;
}

void *List::next() {
	node *tmp = iter;
	if(iter)
		iter = iter->next;
	return tmp->obj;
}

bool List::hasNext() {
	return iter != 0;
}


List::~List() {
	clear();
}
