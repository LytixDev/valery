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

#include "../valery.h"

#ifndef PROMPT
#define PROMPT

#define ARROW_KEY 27
#define ARROW_KEY_2 91
#define ARROW_UP 65
#define ARROW_DOWN 66
#define ARROW_RIGHT 67
#define ARROW_LEFT 68

#define up(x) printf("\033[xA");
#define down(x) printf("\033[xB");
#define right(x) printf("\033[xC");
#define left(x) printf("\033[2D");
#define clear() printf("\033[2J");
#define erase_line() printf("\33[2K\r");

void split_buffer(char *buf, char *cmd, char *args);
void clear_buffer(char *buf);
void print_buffer_with_ps1(char *ps1, char *buf);
int consume_arrow_key();
int prompt(char *ps1, char buf[COMMAND_LEN]);

#endif
