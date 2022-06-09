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
#define MAX_COMMANDS_BEFORE_WRITE 50
#define READ_FROM_MEMORY 0
#define READ_FROM_HIST 1
#define DID_NOT_READ 2


/* types */

/*
 * Holds recently typed in commands in and a connection to the hist file for previously
 * typed in commands.
 * 
 * pos is initialized to total_stored_commands + f_len and can move up to zero.
 * f_pos is initialized to f_len.
 * 
 * Operations on HISTORY:
 * HIST_UP: decrements pos by one.
 * HIST_DOWN: increments pos by one.
 *
 * use init_history() and free_history() to create and free HISTORY types.
 */
typedef struct HISTORY {
    FILE *fp;
    /* newest last */
    char **stored_commands;
    /* total stored commands in memory */
    size_t t_stored;
    /* absolute position in history queue */
    size_t pos;
    /* position of file pointer in history queue */
    size_t f_pos;
    /* amount of lines/commands in hist file */
    size_t f_len;
} HISTORY;

/* functions */
struct HISTORY *init_history(char *full_pat_to_hist_file);
void free_history(struct HISTORY *hist);
void reset_hist_pos(struct HISTORY *hist);
void save_command(struct HISTORY *hist, char buf[COMMAND_LEN]);
void write_commands_to_hist_file(struct HISTORY *hist);
int get_len(FILE *fp);
int out_of_bounds(struct HISTORY *hist, int action);
int open_hist_file(struct HISTORY *hist, char *path);
void read_line_and_move_fp_back(FILE *fp, long offset, char buf[COMMAND_LEN]);
void reset_hist_pos(struct HISTORY *hist);
int get_hist_line(struct HISTORY *hist, char buf[COMMAND_LEN], int action);

#endif
