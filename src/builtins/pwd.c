/*
 *  Prints the current working directory.
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

#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <limits.h>
#include <stdbool.h>

#include "builtins/builtins.h"


int pwd(char result[4096])
{
    bool interactive = result == NULL ? true : false;
    char cwd[4096];

    if (getcwd(cwd, sizeof(cwd)) != NULL) {
        if (interactive)
            printf("%s\n", cwd);
        else
            strncpy(result, cwd, 4096);
    } else {
        fprintf(stderr, "valery internal error: call to getcwd() failed\n");
        return 1;
    }

    return 0;
}
