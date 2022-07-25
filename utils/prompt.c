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


int get_arrow_type()
{
    /*
       Arrow keys takes up three chars in the buffer.
       Only last char codes for up or down, so consume
       second char value.
     */
    if (getchar() == ARROW_KEY_2)
        return getchar();
   
    /* consume and discard next char */
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

/* inserts char at any point in the char array */
void insert_char_to_str(char buf[COMMAND_LEN], char c, int index)
{
    char tmp[COMMAND_LEN];
    strncpy(tmp, buf, index);
    tmp[index] = c;
    strcpy(tmp + index + 1, buf + index); 
    strcpy(buf, tmp);
}

void remove_char_from_str(char buf[COMMAND_LEN], int index)
{
    char tmp[COMMAND_LEN];
    strncpy(tmp, buf, index - 1);
    strcpy(tmp + index - 1, buf + index); 
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

int prompt(struct hist_t *hist, char *ps1, char buf[COMMAND_LEN])
{
    int ch;
    int arrow_type;
    size_t new_buf_len;
    size_t max_len = COMMAND_LEN;
    readfrom_t rc;
    histaction_t action;
    size_t cur_pos = 0;
    /* the size of the buffer being used, not the size allocated */
    size_t buf_len = 0;

    print_prompt(ps1, buf);
    /* reset position in history to bottom of queue */
    reset_hist_pos(hist);

    while (EOF != (ch = getchar()) && ch != '\n') {
        /* return if buffer cannot store more chars */
        if (buf_len == max_len) {
            buf[--buf_len] = 0;
            return 1;
        }

        switch (ch) {
            case BACKSPACE:
                if (cur_pos > 0) {
                    remove_char_from_str(buf, cur_pos);
                    cur_pos--;
                    buf_len--;
                }
                break;

            case ARROW_KEY:
                arrow_type = get_arrow_type();
                if (arrow_type == ARROW_LEFT || arrow_type == ARROW_RIGHT) {
                    cur_pos = move_cursor_horizontally(arrow_type, cur_pos, buf_len);
                    break;
                }

                /* execution enters here means either arrow up or down was pressed */
                /* store hist line inside buf */
                action = (arrow_type == ARROW_UP) ? HIST_UP : HIST_DOWN;
                rc = hist_t_get_line(hist, buf, action);

                if (rc == DID_NOT_READ && action == HIST_DOWN) {
                    /* clear buffer when no hist line was read */
                    memset(buf, 0, COMMAND_LEN);
                    buf_len = cur_pos = 0;
                } else if (rc == READ_FROM_HIST) {
                    /* chop off newline character */
                    buf_len = strlen(buf);
                    buf[--buf_len] = 0;
                    cur_pos = buf_len;
                } else if (rc == READ_FROM_MEMORY) {
                    buf_len = strlen(buf);
                    cur_pos = buf_len;
                }
                break;

            default:
                if (cur_pos != buf_len) {
                    /* insert char at any position of the buffer */
                    insert_char_to_str(buf, ch, cur_pos++); 
                    buf_len++;
                } else {
                    /* no special keys have been pressed this session, so append char to input buffer */
                    buf[cur_pos++] = ch;
                    buf_len++;
                }
        }
        update_prompt(ps1, buf, buf_len - cur_pos);
    }

    putchar('\n');
    return 0;
}
