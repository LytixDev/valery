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

#define MAX_COMMANDS_BEFORE_WRITE 50


/* types */
typedef enum {
    HIST_UP,
    HIST_DOWN
} histaction_t;

typedef enum {
    READ_FROM_MEMORY,
    READ_FROM_HIST, 
    DID_NOT_READ 
} readfrom_t;

/*
 * Holds recently typed in commands in and a connection to the hist file for previously
 * typed in commands.
 * 
 * pos is initialized to total_stored_commands + f_len and can move up to zero.
 * f_pos is initialized to f_len.
 * 
 * Operations on hist_t:
 * HIST_UP: decrements pos by one.
 * HIST_DOWN: increments pos by one.
 *
 * use init_history() and free_history() to create and free hist_t types.
 */
typedef struct hist_t {
    FILE *fp;
    /* newest last */
    char **stored_commands;
    /* total stored commands in memory */
    size_t s_len;
    /* absolute position in history queue */
    size_t pos;
    /* position of file pointer in history queue */
    size_t f_pos;
    /* amount of lines in hist file */
    size_t f_len;
    /* amount of chars in hist file */
    size_t f_chars;
} hist_t;


/* functions */

/* returns a pointer of type hist_t with malloced data */
struct hist_t *malloc_history(char *full_pat_to_hist_file);

/* frees the data associated with the hist_t pointer passed in */
void free_history(struct hist_t *hist);

/* moves file pointer to the end and resets position counters */
void reset_hist_pos(struct hist_t *hist);

/* stores the input buffer into memory */
void save_command(struct hist_t *hist, char buf[COMMAND_LEN]);

/* 
 * writes the stored commands to the hist file connection and clears
 * data from memory.
 */
void write_commands_to_hist_file(struct hist_t *hist);

/* 
 * returns the amount of lines in the file and in the process moves
 * the file pointer to the end of the file.
 */
void count_hist_lines(struct hist_t *hist);

/*
 * traverses the hist one line in the given direction.
 * NB: function does not make sure direction does not cause
 * hist to go out of bounds.
 */
long traverse_hist(struct hist_t *hist, histaction_t direction);

/* returns 1 if the given action will put the file pointer out of
 * bounds, else 0.
 */
int out_of_bounds(struct hist_t *hist, histaction_t action);

/*
 * opens a read and write connection to the hist file.
 * creates a new hist file if input path does not exist.
 * returns 1 if no connection could be made, else 0.
 */
int open_hist_file(struct hist_t *hist, char *path);

/*
 * reads one line of the hist file and moves the file pointer back to the given offset.
 * if offset is -1, it uses ftell(hist->fp) to get the offset
 */
void read_hist_line_from_file(struct hist_t *hist, char buf[COMMAND_LEN], long offset, histaction_t action);

/*
 * puts the current hist line into the buf argument.
 * returns where it got the hist line from (see definitions on the 
 * top of the file).
 */
readfrom_t get_hist_line(struct hist_t *hist, char buf[COMMAND_LEN], histaction_t action);

#endif
