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

#ifndef HISTFILE
#define HISTFILE

#include <stdio.h>

#include "valery.h"

#define MAX_COMMANDS_BEFORE_WRITE 50


/* types */
typedef enum {
    HIST_UP,
    HIST_DOWN
} histaction_t;

typedef enum {
    READ_FROM_MEMORY = -1,
    READ_FROM_HIST = -2, 
    DID_NOT_READ = -3 
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
 * use init_history() and hist_t_free() to create and free hist_t types.
 */
typedef struct hist_t {
    FILE *fp;
    char **stored_commands; /* newest last */
    size_t s_len;           /* total stored commands in memory */
    size_t pos;             /* absolute position in history queue */
    size_t f_pos;           /* position of file pointer in history queue */
    size_t f_len;           /* amount of lines in hist file */
    long f_chars;           /* amount of chars in hist file */
} hist_t;


/* functions */

/* returns a pointer of type hist_t with malloced data */
struct hist_t *hist_t_malloc(char *full_pat_to_hist_file);

/* frees the data associated with the hist_t pointer passed in */
void hist_t_free(struct hist_t *hist);

/* moves file pointer to the end and resets position counters */
void hist_t_reset_pos(struct hist_t *hist);

/* stores the input buffer into memory */
void hist_t_save(struct hist_t *hist, char buf[COMMAND_LEN]);

/* 
 * writes the stored commands to the hist file connection and clears
 * data from memory.
 */
void hist_t_write(struct hist_t *hist);

/* 
 * stores the amount of lines in the file and in the process moves
 * the file pointer to the end of the file.
 */
void hist_t_count(struct hist_t *hist);

/*
 * traverses the hist one line in the given direction.
 * NB: function does not make sure direction does not cause
 * hist to go out of bounds.
 */
long hist_t_traverse(struct hist_t *hist, histaction_t direction);

/* returns 1 if the given action will put the file pointer out of
 * bounds, else 0.
 */
int out_of_bounds(struct hist_t *hist, histaction_t action);

/*
 * opens a read and write connection to the hist file.
 * creates a new hist file if input path does not exist.
 * returns 1 if no connection could be made, else 0.
 */
int hist_t_open(struct hist_t *hist, char *path);

/*
 * reads one line of the hist file and moves the file pointer back to the given offset.
 * if offset is -1, it uses ftell(hist->fp) to get the offset
 */
void hist_t_read_line_f(struct hist_t *hist, char buf[COMMAND_LEN], long offset);

/*
 * puts the current hist line into the buf argument.
 * returns where it got the hist line from (see definitions on the 
 * top of the file).
 */
readfrom_t hist_t_get_line(struct hist_t *hist, char buf[COMMAND_LEN], histaction_t action);

#endif
