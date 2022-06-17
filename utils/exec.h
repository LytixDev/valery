/*
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


#ifndef EXEC
#define EXEC

#include "lexer.h"

#define CHILD_PID 0

/* functions */

/* TODO: Update description
 * forks the process and executes a program with the given
 * path and args.
 * returns 0 if succesfull else 1.
 */
int valery_exec_program(char *path, char *args, int stream[2]);

int valery_exec_buffer(struct tokens_t *tokens);

#endif
