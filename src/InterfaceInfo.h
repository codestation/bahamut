/*
 *  Project Bahamut: full ad-hoc tunneling software to be used by the
 *  Playstation Portable (PSP) to emulate online features.
 *
 *  Copyright (C) 2008  Project Bahamut team
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
 *     Holds the name/description of a network interface
 * Special Notes:
 *		TODO: none yet
 */

#ifndef INTERFACEINFO_H_
#define INTERFACEINFO_H_

#include <string.h>
#include <stdlib.h>

class InterfaceInfo
{
public:
	char *name;
	char *desc;
	InterfaceInfo(char *, char *);
	virtual ~InterfaceInfo();
};

#endif /*INTERFACEINFO_H_*/
