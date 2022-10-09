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

#ifndef PROMPT
#define PROMPT

#include <termios.h>
#include "valery/histfile.h"
#include "valery/env.h"

/* types */
struct termconf_t {
    struct termios original;
    struct termios new;
};

struct prompt_t {
    char *buf;
    unsigned int buf_size;
    unsigned int buf_capacity;
    unsigned int cursor_position;
    struct termconf_t *termconf;
};

/* handles all the logic when receiving input from the user */
void prompt(struct prompt_t *prompt, struct hist_t *hist, char *ps1);

struct prompt_t *prompt_malloc(void);

void prompt_free(struct prompt_t *prompt);

#endif
