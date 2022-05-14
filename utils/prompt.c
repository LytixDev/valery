/*
 *  Displays the PS1 along with a prompt for the user to type in commands.
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

#include <stdio.h>
#include <stdlib.h>

#include "prompt.h"


char *prompt(char *ps1)
{
    printf("%s ", ps1);
    char *str;
    int ch;
    size_t cur_len = 0;
    size_t max_len = CHUNK;

    str = realloc(NULL, sizeof(*str) * max_len);

    while (EOF != (ch = fgetc(stdin)) && ch != '\n') {
        str[cur_len++] = ch;

        if (cur_len == max_len)
            str = realloc(str, sizeof(*str) * (max_len += CHUNK));
    }
    /* add null byte to terminate string */
    str[cur_len++] = '\0';
    
    return realloc(str, sizeof(*str) * cur_len);
}
