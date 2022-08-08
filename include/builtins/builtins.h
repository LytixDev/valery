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

#include <stdbool.h>

#include "valery/histfile.h"

#define COMMAND_IN_PATH         0
#define COMMAND_NOT_FOUND       1
#define COMMAND_IS_BUILTIN      2
#define COMMAND_IS_PATH         3

#define total_builtin_functions 5
extern char *builtin_names[total_builtin_functions];


/* functions */

/*
 * which builtin that is meant to be used interactively.
 * calls which_single() on all program_names and prints where program executable
 * is located, what type of program it is, or could not find.
 * returns 0 if all program were found, else 1.
 */
int which(char **program_names, int program_count, char **paths, int path_count);

/*
 * if which is used interactively, pass NULL as the path_result parameter.
 * if path_result is not NULL, assumes the function is not used interactively.
 * in this case, the program does not print the result, but instead puts the 
 * address of the path from paths into path_result if it is found.
 * returns COMMAND_IN_PATH, COMMAND_IS_BUILTIN and COMMAND_NOT_FOUND accordingly.
 */
int which_single(char *program_name, char **paths, int path_count, char **path_result);

/*
 * its cd
 */
int cd(char *directory);

/*
 * if print_all is true, program prints entire hist file.
 * if not, program prints the 15 most recent hist lines.
 */
int history(struct hist_t *hist, bool print_all);

/*
 * if result is NULL, program prints the current working directory.
 * if not, program copies the current working directory into result param.
 */
int pwd(char result[4096]);

int help();


#endif
