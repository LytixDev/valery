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

/* returns a pointer of type HISTORY with malloced data */
struct HISTORY *init_history(char *full_pat_to_hist_file);

/* frees the data associated with the HISTORY pointer passed in */
void free_history(struct HISTORY *hist);

/* moves file pointer to the end and resets position counters */
void reset_hist_pos(struct HISTORY *hist);

/* stores the input buffer into memory */
void save_command(struct HISTORY *hist, char buf[COMMAND_LEN]);

/* 
 * writes the stored commands to the hist file connection and clears
 * data from memory.
 */
void write_commands_to_hist_file(struct HISTORY *hist);

/* 
 * returns the amount of lines in the file and in the process moves
 * the file pointer to the end of the file.
 */
int get_len(FILE *fp);

/* returns 1 if the given action will put the file pointer out of
 * bounds, else 0.
 */
int out_of_bounds(struct HISTORY *hist, int action);

/*
 * opens a read and write connection to the hist file.
 * creates a new hist file if input path does not exist.
 * returns 1 if no connection could be made, else 0.
 */
int open_hist_file(struct HISTORY *hist, char *path);

/* reads the current line and moves the file pointer back to its offset */
void read_line_and_move_fp_back(FILE *fp, long offset, char buf[COMMAND_LEN]);

/*
 * puts the current hist line into the buf argument.
 * returns where it got the hist line from (see definitions on the 
 * top of the file).
 */
int get_hist_line(struct HISTORY *hist, char buf[COMMAND_LEN], int action);

#endif
