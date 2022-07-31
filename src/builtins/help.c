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

#include <stdio.h>

#include "builtins/builtins.h"

int help()
{
    printf("valery - Unix-like shell written by Nicolai Brand (https://lytix.dev) 2022\n"
           "\nThe goal of the project is to be a playground in order to learn how to write memory-safe, "
           "efficient, readable and useful C code.\n");

    printf("\nOn startup, valery reads the '.valeryrc' file in the $HOME folder to customize the environment."
           "Typed in commands are stored in '.valery_hist' in the $HOME folder.\n");

    printf("\nList of shell builtins:\n");
    for (int i = 0; i < total_builtin_functions; i++) {
        printf("%s  ", builtin_names[i]);
    }

    printf("\n\nUse the -c option to execute a command directly when envoking valery. Example: './valery -c \"ls\"'\n");

    printf("\n");
    return 0;
}
