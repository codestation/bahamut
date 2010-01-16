/*
 *  Project Bahamut: full ad-hoc tunneling software to be used by the
 *  Playstation Portable (PSP) to emulate online features.
 *
 *  Copyright (C) 2008  Codestation
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

/*
 * File Description:
 *     Generic list to store data (avoid using the bloated STL library :D )
 * Special Notes:
 *		TODO: some error checking
 */

#ifndef LIST_H_
#define LIST_H_

class List {
private:
	typedef int (*COMPARE_FUNC) (void *, void *);
	typedef void (*DELETE_FUNC) (void *);
	COMPARE_FUNC comp;
	DELETE_FUNC del;
	struct node {
		void *obj;
		node *next;
	} *head, *iter;
	int counter;
public:
	List(COMPARE_FUNC, DELETE_FUNC);
	void add(void *item);
	void *get(void *);
	void *getByIndex(int i);
	bool exist(void *);
	bool remove(void *);
	int count();
	void clear();
	void rewind();
	void *next();
	bool hasNext();
	virtual ~List();
};

#endif /*LIST_H_*/
