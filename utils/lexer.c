/*
 *  Humble lexer for shell commands.
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
#include <string.h>


void split_buffer(char *buffer, char *cmd, char *args)
{
    const char delim[] = " ";

    char *cmd_tmp = strtok(buffer, delim);
    char *args_tmp = strtok(NULL, delim);

    if (cmd_tmp != NULL)
        strcpy(cmd, cmd_tmp);

    if (args_tmp != NULL)
        strcpy(args, args_tmp);
}
