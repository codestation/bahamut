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

#ifndef LOGGER_H_
#define LOGGER_H_

#include <stdio.h>

extern int log_enabled;

#define INFO_ON() log_enabled = 1;
#define INFO_OFF() log_enabled = 0;
#define INFO(format, ...) if(log_enabled) printf(format,  ## __VA_ARGS__)
#define ERR(format, ...) fprintf(stderr, format,  ## __VA_ARGS__)

#endif /* LOGGER_H_ */
