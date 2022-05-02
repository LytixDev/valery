/*
 *  <one line to give the program's name and a brief idea of what it does.>
 *   
 *  Copyright (C) 2022 Nicolai Brand 
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
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include <stdint.h>

#ifndef VALERY
#define VALERY

/* types */
typedef struct ENV {
    char *PS1;
    char *PATH;
} ENV;

/* variables */
#define COMMAND_LEN 255

/* functions */
void disp(uint8_t *ps1, char *ret);
struct ENV *new_env();
void free_env(struct ENV *env);

#endif
