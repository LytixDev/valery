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
#include "histfile.h"
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

int get_arrow_type()
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

void clear_buffer(char buf[COMMAND_LEN])
{
    memset(buf, 0, COMMAND_LEN);
}

void init_prompt(char *ps1, char *buf)
{
    printf("%s %s", ps1, buf);
}

void update_prompt(char *ps1, char *buf, int cur_pos)
{
    flush_line();
    printf("%s %s", ps1, buf);
    int cursor_position = cur_pos + strlen(ps1) + 1;
    // TODO: need to split buffer / shift all chars to the right of newly typed in char
    //cursor_goto(cursor_position);
}

int prompt(struct HIST_FILE *hf, char *ps1, char buf[COMMAND_LEN])
{
    int ch;
    int arrow_type;
    size_t new_buf_len;
    size_t cur_pos = 0;
    size_t max_len = COMMAND_LEN;

    init_prompt(ps1, buf);
    while (EOF != (ch = getchar()) && ch != '\n') {
        if (cur_pos == max_len) {
            buf[cur_pos] = 0;
            return 1;
        }

        if (ch == BACKSPACE && cur_pos > 0) {
            buf[--cur_pos] = 0;
        } else if (ch == ARROW_KEY) {
            arrow_type = get_arrow_type();
            if (arrow_type == ARROW_LEFT) {
                cursor_left(1);
                cur_pos--;
                continue;
            }

            /* store hist line inside buf */
            read_hist_line(hf, buf, (arrow_type == ARROW_UP) ? HIST_UP : HIST_DOWN);
            new_buf_len = strlen(buf);
            /* chop off newline character and decrement length */
            buf[--new_buf_len] = 0;
            cur_pos = new_buf_len;
            
        } else {
            buf[cur_pos++] = ch;
        }

        update_prompt(ps1, buf, cur_pos);
    }

    return 0;
}
