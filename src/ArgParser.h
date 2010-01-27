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

#ifndef ARGPARSER_H_
#define ARGPARSER_H_

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

class ArgParser {
private:
	char *host;
	char *port;
	bool list_interfaces;
	bool verbose;
	bool disable_order;
	char *interf;
	bool dedicated;
	bool empty;
public:
	ArgParser();
	bool parse(int argc, char **argv);
	inline const char *getHost() { return host; }
	inline const char *getPort() { return port; }
	inline bool listInterfaces() { return list_interfaces; }
	inline bool verboseMode() { return verbose; }
	inline const char *interfaceName() { return interf; }
	inline bool disableOrdering() { return disable_order; }
	inline bool dedicatedServer() { return dedicated; }
	inline bool isEmpty() { return empty; }
	virtual ~ArgParser();
};

#endif /* ARGPARSER_H_ */
