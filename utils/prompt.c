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
    return -1;
}

int move_cursor_horizontally(keycode_t arrow_type, int cur_pos, int buf_len)
{
    if (arrow_type == ARROW_LEFT) {
        if (cur_pos < 1) return cur_pos;
        cursor_left(1);
        return --cur_pos;
    }

    if (arrow_type == ARROW_RIGHT) {
        if (cur_pos >= buf_len) return cur_pos;
        cursor_right(1);
        return ++cur_pos;
    }

    return cur_pos;
}

/* inserts chars at any point in the char array */
void insert_char_to_str(char buf[COMMAND_LEN], char c, int index)
{
    char tmp[COMMAND_LEN];
    strncpy(tmp, buf, index);
    tmp[index] = c;
    strcpy(tmp + index + 1, buf + index); 
    strcpy(buf, tmp);
}

void print_prompt(char *ps1, char *buf)
{
    printf("%s %s", ps1, buf);
}

void update_prompt(char *ps1, char *buf, int cursor_pos)
{
    flush_line();
    print_prompt(ps1, buf);
    cursor_left(cursor_pos);
    /* if cursor at the end of buffer, move cursor one to the right */
    if (cursor_pos == 0)
        cursor_right(1);
}

int prompt(struct HISTORY *hist, char *ps1, char buf[COMMAND_LEN])
{
    int ch;
    int arrow_type;
    size_t new_buf_len;
    size_t cur_pos = 0;
    size_t max_len = COMMAND_LEN;
    readfrom_t rc;
    histaction_t action;

    print_prompt(ps1, buf);
    /* reset position in history to bottom of queue */
    reset_hist_pos(hist);

    while (EOF != (ch = getchar()) && ch != '\n') {
        /* return if buffer cannot store more chars */
        if (cur_pos == max_len) {
            buf[cur_pos] = 0;
            return 1;
        }

        if (ch == BACKSPACE && cur_pos > 0) {
            buf[--cur_pos] = 0;
        } else if (ch == ARROW_KEY) {
            arrow_type = get_arrow_type();
            if (arrow_type == ARROW_LEFT || arrow_type == ARROW_RIGHT) {
                cur_pos = move_cursor_horizontally(arrow_type, cur_pos, strlen(buf));
                continue;
            }

            /* store hist line inside buf */
            action = (arrow_type == ARROW_UP) ? HIST_UP : HIST_DOWN;
            rc = get_hist_line(hist, buf, action);

            if (rc != DID_NOT_READ)
                action == HIST_UP ? hist->pos-- : hist->pos++;

            if (rc == READ_FROM_HIST) {
                /* chop off newline character */
                new_buf_len = strlen(buf);
                buf[--new_buf_len] = 0;
                cur_pos = new_buf_len;
            } else if (rc == READ_FROM_MEMORY) {
                new_buf_len = strlen(buf);
                cur_pos = new_buf_len;
            }

        } else {
            if (cur_pos != strlen(buf)) {
                insert_char_to_str(buf, ch, cur_pos++); 
            } else {
                /* no special keys have been pressed this session, so append char to input buffer */
                buf[cur_pos++] = ch;
            }
        }

        update_prompt(ps1, buf, strlen(buf) - cur_pos);
    }

    return 0;
}
