/*
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

#include "../valery.h"

#ifndef HISTFILE
#define HISTFILE

#define _GNU_SOURCE
#define HIST_UP 0
#define HIST_DOWN 1
#define MAX_COMMANDS_BEFORE_WRITE 3

/* types */
typedef struct HISTORY {
    FILE *fp;
    char **stored_commands;
    size_t current_line;
    size_t f_current_line;
    size_t f_len;
    size_t total_stored_commands;
} HISTORY;

/* functions */
struct HISTORY *init_history();
void free_history(struct HISTORY *hist);
void save_command(struct HISTORY *hist, char buf[COMMAND_LEN]);
void write_commands_to_hist_file(struct HISTORY *hist);
int get_len(FILE *fp);
/* returns 0 if it can open hist file, else 1 */
int open_hist_file(struct HISTORY *hist, char *full_path);
size_t read_line_and_move_fp_back(FILE *fp, long offset, char buf[COMMAND_LEN]);
void update_current_line(struct HISTORY *hist, int action);
size_t get_hist_line(struct HISTORY *hist, char buf[COMMAND_LEN], int action);

#endif
