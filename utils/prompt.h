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

#include "histfile.h"
#include "../valery.h"

#ifndef PROMPT
#define PROMPT

#define ARROW_KEY 27
#define ARROW_KEY_2 91
#define ARROW_UP 65
#define ARROW_DOWN 66
#define ARROW_RIGHT 67
#define ARROW_LEFT 68

#define BACKSPACE 127

/* macros for moving the cursor horizontally */
#define cursor_right(n) printf("\033[%dC", (n));
#define cursor_left(n) printf("\033[%dD", (n));
#define cursor_goto(x) printf("\033[%d", (x));
#define flush_line() printf("\33[2K\r");


/* functions */

/*
 * splits the input buffer on the first space:
 * - first part stored into cmd argument
 * - second part stored into args argument
 */
void split_buffer(char *buf, char *cmd, char *args);

/* returns the type of arrow consumed from the terminal input buffer */
int get_arrow_type();

int move_cursor_horizontally(int arrow_type, int cur_pos, int buf_len);

/* merge a char at any position into a char array */
void insert_char_to_str(char buf[COMMAND_LEN], char c, int index);

/* prints the ps1 and the buffer */
void print_prompt(char *ps1, char *buf);

/*
 * flushes the screen, prints the ps1 and buffer and moves the
 * cursor to the end of the buffer.
 */
void update_prompt(char *ps1, char *buf, int cursor_pos);

/* handles all the logic when receiving input from the user */
int prompt(struct HISTORY *hist, char *ps1, char buf[COMMAND_LEN]);

#endif
