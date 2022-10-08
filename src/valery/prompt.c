/*
 *  Displays a prompt for the user to type in commands. Operates on the
 *  buffer inputted by the user.
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
#include <unistd.h>
#include <termios.h>

#include "valery.h"
#include "valery/prompt.h"
#include "valery/histfile.h"
#include "lib/vstring.h"


extern struct termios originalt, newt;


/* macros for moving the cursor horizontally */
#define cursor_right(n) printf("\033[%dC", (n));
#define cursor_left(n) printf("\033[%dD", (n));
#define cursor_goto(x) printf("\033[%d", (x));
#define flush_line() printf("\33[2K\r");


/* types */
enum keycode_t {
    ARROW_KEY = 27,
    ARROW_KEY_2 = 91,
    ARROW_UP = 65,
    ARROW_DOWN = 66,
    ARROW_RIGHT = 67,
    ARROW_LEFT = 68,

    BACKSPACE = 127
};


static void prompt_term_init(void)
{
    tcgetattr(STDIN_FILENO, &originalt);
    newt = originalt;
    /* change so buffer don't require new line or similar to return */
    newt.c_lflag &= ~ICANON;
    newt.c_cc[VTIME] = 0;
    newt.c_cc[VMIN] = 1;
    /* turn of echo */
    newt.c_lflag &= ~ECHO;
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
}

static inline void prompt_term_end(void)
{
    tcsetattr(STDIN_FILENO, TCSANOW, &originalt);
}

/* returns the type of arrow consumed from the terminal input buffer */
static int get_arrow_type(void)
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

static int move_cursor_horizontally(enum keycode_t arrow_type, int cur_pos, int buf_len)
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

static inline void print_prompt(char *ps1, char *buf)
{
    printf("%s %s", ps1, buf);
}

/*
 * flushes the screen, prints the ps1 and buffer and moves the
 * cursor to the end of the buffer.
 */
static void update_prompt(char *ps1, char *buf, int cursor_pos)
{
    flush_line();
    print_prompt(ps1, buf);
    cursor_left(cursor_pos);
    /* if cursor at the end of buffer, move cursor one to the right */
    if (cursor_pos == 0)
        cursor_right(1);
}

void prompt(struct hist_t *hist, char *ps1, char buf[COMMAND_LEN])
{
    int ch;
    int arrow_type;
    size_t max_len = COMMAND_LEN;
    readfrom_t rc;
    histaction_t action;
    size_t cur_pos = 0;
    /* the size of the buffer being used, not the size allocated */
    size_t buf_len = 0;

    prompt_term_init();
    print_prompt(ps1, buf);
    /* reset position in history to bottom of queue */
    hist_t_reset_pos(hist);

    while (EOF != (ch = getchar()) && ch != '\n') {
        /* return if buffer cannot store more chars */
        if (buf_len == max_len) {
            buf[--buf_len] = 0;
            goto ret;
        }

        switch (ch) {
            case BACKSPACE:
                if (cur_pos > 0) {
                    vstr_remove_idx(buf, COMMAND_LEN, cur_pos);
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
                    vstr_insert_c(buf, COMMAND_LEN, ch, cur_pos++);
                    buf_len++;
                } else {
                    /* no special keys have been pressed this session, so append char to input buffer */
                    buf[cur_pos++] = ch;
                    buf_len++;
                }
        }
        update_prompt(ps1, buf, buf_len - cur_pos);
    }

ret:
    putchar('\n');
    prompt_term_end();
}
