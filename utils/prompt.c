/*
 *  Displays a prompt for the user to type in commands. Operates on the
 *  buffer inputet by the user.
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
#include <stdlib.h>

#include "prompt.h"
#include "../valery.h"


void split_buffer(char *buf, char *cmd, char *args)
{
    const char delim[] = " ";

    char *cmd_tmp = strtok(buf, delim);
    char *args_tmp = strtok(NULL, delim);

    if (cmd_tmp != NULL)
        strcpy(cmd, cmd_tmp);

    if (args_tmp != NULL)
        strcpy(args, args_tmp);
}

void clear_buffer(char *buf)
{
    buf[0] = '\0';
}

void print_buffer_with_ps1(char *ps1, char *buf)
{
    printf("\n%s %s ", ps1, buf);
}

int consume_arrow_key()
{
    /*
       Arrow keys takes up three chars in the buffer.
       Only last char codes for up or down, so consume
       second char value.
     */
    if (getchar() == ARROW_KEY_2)
        return getchar();
   
    getchar();
    return 0;
}

/*
 * Returns 0 when a normal command is typed in.
 * Returns an arrow key code (see prompt.h) when an arrow key is
 * pressed.
 */
int prompt(char *ps1, char buf[COMMAND_LEN])
{
    int ch;
    size_t cur_pos = 0;
    size_t max_len = COMMAND_LEN;

    printf("%s ", ps1);

    while (EOF != (ch = getchar()) && ch != '\n') {
        if (ch == ARROW_KEY) {
            ch = consume_arrow_key();
            switch (ch) {
                case ARROW_UP:
                    return ARROW_UP;
                case ARROW_DOWN:
                    return ARROW_DOWN;
                case ARROW_RIGHT:
                    return ARROW_RIGHT;
                case ARROW_LEFT:
                    return ARROW_LEFT;
                /* if key not valid arrow directional code, ignore and continue */
            }
        } else {
            if (cur_pos == max_len) {
                buf[cur_pos] = '\0';
                return 1;
            }
                
            putchar(ch);
            buf[cur_pos++] = ch;
        }
    }

    /* terminate string */
    buf[cur_pos] = '\0';
    return 0;
}
