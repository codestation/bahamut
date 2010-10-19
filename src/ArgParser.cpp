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

#include "ArgParser.h"

ArgParser::ArgParser() {
	host = NULL;
	port = NULL;
	list_interfaces = false;
	verbose = 0;
	disable_order = false;
	interf = NULL;
	dedicated = false;
	empty = true;
}

bool ArgParser::parse(int argc, char **argv) {
	int c;
	opterr = 0;
	while ((c = getopt (argc, argv, "h:p:i:vold")) != -1) {
		switch (c) {
		case 'v':
			verbose++;
			break;
		case 'o':
			disable_order = true;
			break;
		case 'l':
			list_interfaces = true;
			break;
		case 'd':
			dedicated = true;
			break;
		case 'i':
			interf = optarg;
			break;
		case 'h':
			host = optarg;
			break;
		case 'p':
			port = optarg;
			break;
		case '?':
			if(optopt == 'i' || optopt == 'h' || optopt == 'p')
				fprintf (stderr, "Option -%c requires an argument.\n", optopt);
			else if (isprint(optopt))
				fprintf (stderr, "Unknown option `-%c'.\n", optopt);
			else
				fprintf (stderr, "Unknown option character `\\x%x'.\n", optopt);
			return false;
		default:
			abort();
		}
		empty = false;
	}
	return !empty;
}

ArgParser::~ArgParser() {
	// TODO Auto-generated destructor stub
}
