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

#ifndef BUILTINS
#define BUILTINS

#include "valery/histfile.h"

#define COMMAND_IN_PATH 0
#define COMMAND_NOT_FOUND 1
#define COMMAND_IS_BUILTIN 2

#define total_builtin_functions 4
extern char *builtin_names[total_builtin_functions];


/* functions */

/*
 * if which is used interactively, pass NULL as the path_result parameter.
 * if path_result is not NULL, assumes the function is not used interactively.
 * in this case, the program does not print the result, but instead puts the 
 * address of the path from paths into path_result if it is found.
 * returns COMMAND_IN_PATH, COMMAND_IS_BUILTIN and COMMAND_NOT_FOUND accordingly.
 */
int which(char *program_name, char **paths, int path_capacity, char **path_result);

int cd(char *directory);

int history(struct hist_t *hist);

int help();

#endif
