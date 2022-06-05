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
typedef struct HIST_FILE {
    FILE *fp;
    size_t current_line;
    size_t len;
} HIST_FILE;

typedef struct HIST_FILE_WRITER {
    char **commands;
    int total_commands;
} HIST_FILE_WRITER;

/* functions */
struct HIST_FILE *new_hist_file();
void free_hist_file(struct HIST_FILE *hf);
struct HIST_FILE_WRITER *new_hist_file_writer();
void free_hist_file_writer(struct HIST_FILE_WRITER *hfw);
void save_command(struct HIST_FILE_WRITER *hfw, struct HIST_FILE *hf, char buf[COMMAND_LEN]);
void write_commands_to_hist_file(FILE *fp, char **commands, int total_commands);
int get_len(FILE *fp);
int open_hist_file(struct HIST_FILE *hf, char *full_path);
int read_line_and_move_fp_back(FILE *fp, long offset, char buf[COMMAND_LEN]);
int read_hist_line(struct HIST_FILE *hf, char buf[COMMAND_LEN], int action);

#endif
