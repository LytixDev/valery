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

#define cursor_right(n) printf("\033[%dC", (n));
#define cursor_left(n) printf("\033[%dD", (n));
#define cursor_goto(x) printf("\033[%d", (x));
#define flush_line() printf("\33[2K\r");

void split_buffer(char *buf, char *cmd, char *args);
int get_arrow_type();
int move_cursor_horizontally(int arrow_type, int cur_pos, int buf_len);
void insert_char_to_str(char buf[COMMAND_LEN], char c, int index);
void init_prompt(char *ps1, char *buf);
void update_prompt(char *ps1, char *buf, int cursor_pos);
int prompt(struct HISTORY *hist, char *ps1, char buf[COMMAND_LEN]);

#endif
