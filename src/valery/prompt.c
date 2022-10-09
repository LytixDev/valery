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


/* macros for moving the cursor horizontally */
#define cursor_right(n) printf("\033[%dC", (n))
#define cursor_left(n) printf("\033[%dD", (n))
#define cursor_goto(x) printf("\033[%d", (x))
#define flush_line() printf("\33[2K\r")


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


static void prompt_debug(struct prompt_t *prompt)
{
    printf("\n--- prompt ---\n");
    printf("buf: '%s'\n", prompt->buf);
    printf("size: '%d' cursor_position: '%d'\n", prompt->buf_size, prompt->cursor_position);
}

static void prompt_term_init(struct termconf_t *termconf)
{
    tcgetattr(STDIN_FILENO, &termconf->original);
    termconf->new = termconf->original;
    /* change so buffer don't require new line or similar to return */
    termconf->new.c_lflag &= ~ICANON;
    termconf->new.c_cc[VTIME] = 0;
    termconf->new.c_cc[VMIN] = 1;
    /* turn of echo */
    termconf->new.c_lflag &= ~ECHO;
    tcsetattr(STDIN_FILENO, TCSANOW, &termconf->new);
}

static inline void prompt_term_end(struct termconf_t *termconf)
{
    tcsetattr(STDIN_FILENO, TCSANOW, &termconf->original);
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

static int move_cursor_horizontally(struct prompt_t *prompt, enum keycode_t arrow_type)
{
    if (arrow_type == ARROW_LEFT) {
        /* do not move left if cursor already at left boundary */
        if (prompt->cursor_position < 1)
            return prompt->cursor_position;
        cursor_left(1);
        return --prompt->cursor_position;
    }

    /* do not move right if cursor already at right boundary */
    if (prompt->cursor_position >= prompt->buf_size)
        return prompt->cursor_position;
    cursor_right(1);
    return prompt->cursor_position;
}

static inline void print_prompt(struct prompt_t *prompt, char *ps1)
{
    printf("%s %s", ps1, prompt->buf);
}

/*
 * flushes the screen, prints the ps1 and buffer and moves the
 * cursor to the end of the buffer.
 */
static void update_prompt(struct prompt_t *prompt, char *ps1)
{
    flush_line();
    print_prompt(prompt, ps1);
    cursor_left(prompt->buf_size - prompt->cursor_position);
    /* if cursor at the end of buffer, move cursor one to the right */
    if (prompt->cursor_position == 0)
        cursor_right(1);
}

void prompt(struct prompt_t *prompt, struct hist_t *hist, char *ps1)
{
    int ch;
    int arrow_type;
    enum readfrom_t read_from;
    enum histaction_t action;

    /* reset position in history to bottom of queue */
    hist_reset_pos(hist);

    prompt_term_init(prompt->termconf);
    print_prompt(prompt, ps1);
    while (EOF != (ch = getchar()) && ch != '\n') {
#ifdef DEBUG_PROMPT
        prompt_debug(prompt);
#endif
        /* return if buffer cannot store more chars */
        if (prompt->buf_size == prompt->buf_capacity) {
            prompt->buf[prompt->buf_capacity - 1] = 0;
            goto prompt_end;
        }

        switch (ch) {
            case BACKSPACE:
                if (prompt->cursor_position > 0) {
                    vstr_remove_idx(prompt->buf, prompt->buf_capacity, prompt->cursor_position);
                    prompt->cursor_position--;
                    prompt->buf_size--;
                }
                break;

            case ARROW_KEY:
                arrow_type = get_arrow_type();
                if (arrow_type == ARROW_LEFT || arrow_type == ARROW_RIGHT) {
                    prompt->cursor_position = move_cursor_horizontally(prompt, arrow_type);
                    break;
                }

                /* 
                 * execution enters here means either arrow up or down was pressed, so we want
                 * to store the corresponding hist line inside buf
                 */
                action = (arrow_type == ARROW_UP) ? HIST_UP : HIST_DOWN;
                read_from = hist_get_line(hist, prompt->buf, action);

                if (read_from == DID_NOT_READ) {
                    break;
                } else if (read_from == READ_FROM_HIST) {
                    prompt->buf_size = strlen(prompt->buf);
                    /* chop off newline character */
                    prompt->buf[--prompt->buf_size] = 0;
                    prompt->cursor_position = prompt->buf_size;
                } else if (read_from == READ_FROM_MEMORY) {
                    prompt->buf_size = strlen(prompt->buf);
                    prompt->cursor_position = prompt->buf_size;
                }
                break;

            default:
                if (prompt->cursor_position != prompt->buf_size) {
                    /* insert char at any position of the buffer */
                    vstr_insert_c(prompt->buf, COMMAND_LEN, ch, prompt->cursor_position++);
                    prompt->buf_size++;
                } else {
                    /* no special keys have been pressed this session, so append char to input buffer */
                    prompt->buf[prompt->buf_size++] = ch;
                    prompt->cursor_position++;
                }
        }
        update_prompt(prompt, ps1);
    }

prompt_end:
    putchar('\n');
    prompt_term_end(prompt->termconf);
}

struct prompt_t *prompt_malloc(void)
{
    struct prompt_t *prompt = malloc(sizeof(struct prompt_t));
    prompt->termconf = malloc(sizeof(struct termconf_t));
    prompt->buf = malloc(sizeof(char) * COMMAND_LEN);
    prompt->buf_capacity = COMMAND_LEN;
    prompt->buf_size = 0;
    prompt->cursor_position = 0;

    return prompt;
}

void prompt_free(struct prompt_t *prompt)
{
    free(prompt->buf);
    free(prompt->termconf);
    free(prompt);
}

