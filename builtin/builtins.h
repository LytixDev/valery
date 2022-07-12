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

#include "../utils/histfile.h"

#ifndef BUILTINS
#define BUILTINS

#define total_builtin_functions 4
extern char *builtin_names[total_builtin_functions];


/* functions */
int which(char *program_name, char **paths, int total_paths);

int cd(char *directory);

int history(struct hist_t *hist);

int help();

#endif
