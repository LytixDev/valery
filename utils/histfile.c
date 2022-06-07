/*
 *  Operations on history file that stores previously used commands
 *  in succesive order.
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

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>

#include "histfile.h"
#include "../valery.h"


struct HIST_FILE *new_hist_file()
{
    struct HIST_FILE *hf = (HIST_FILE *) malloc(sizeof(HIST_FILE));
    hf->fp = (FILE *) malloc(sizeof(FILE));
    hf->current_line = 0;
    hf->len = 0;
    return hf;
}

void free_hist_file(struct HIST_FILE *hf)
{
    if (hf != NULL) {
        fclose(hf->fp);
        free(hf);
    }
}

struct HIST_FILE_WRITER *new_hist_file_writer()
{
    struct HIST_FILE_WRITER *hfw = (HIST_FILE_WRITER *) malloc(sizeof(HIST_FILE_WRITER));
    hfw->commands = malloc(MAX_COMMANDS_BEFORE_WRITE * sizeof(char *));
    /* allocate space for all strings */
    for (int i = 0; i < MAX_COMMANDS_BEFORE_WRITE; i++)
        hfw->commands[i] = malloc(COMMAND_LEN * sizeof(char));

    hfw->total_commands_stored = 0;
    return hfw;
}

void free_hist_file_writer(struct HIST_FILE_WRITER *hfw)
{
    if (hfw == NULL)
        return;

    for (int i = 0; i < MAX_COMMANDS_BEFORE_WRITE; i++)
        free(hfw->commands[i]);

    free(hfw->commands);
    free(hfw);
}

void save_command(struct HIST_FILE_WRITER *hfw, struct HIST_FILE *hf, char buf[COMMAND_LEN])
{
    if (hfw->total_commands_stored == MAX_COMMANDS_BEFORE_WRITE)
        write_commands_to_hist_file(hf, hfw);
    strncpy(hfw->commands[hfw->total_commands_stored++], buf, COMMAND_LEN);
}

void write_commands_to_hist_file(struct HIST_FILE *hf, struct HIST_FILE_WRITER *hfw)
{
    for (int i = 0; i < hfw->total_commands_stored; i++) {
        if (strcmp(hfw->commands[i], "") != 0) {
            fputs(hfw->commands[i], hf->fp);
            fputs("\n", hf->fp);
            hf->len++;
        }
    }
    /* reset total_commands_stored counter */
    hfw->total_commands_stored = 0;
    hf->current_line = hf->len - 1;
    /* move file pointer back to end of file */
    fseek(hf->fp, 0, SEEK_END);
}

int get_len(FILE *fp)
{
    unsigned int len = 0;

    while(!feof(fp)) {
      if(fgetc(fp) == '\n')
        len++;
    }

    return len;
}

/* returns 0 if it can open hist file, else 1 */
int open_hist_file(struct HIST_FILE *hf, char *full_path)
{
    hf->fp = fopen(full_path, "a+");
    if (hf->fp != NULL) {
        /* set current line to read from to last line of file (0 indexed) */
        hf->len = get_len(hf->fp);
        hf->current_line = hf->len - 1;
        return 0;
    }

    return 1;
}

size_t read_line_and_move_fp_back(FILE *fp, long offset, char buf[COMMAND_LEN])
{
    size_t len = 0;
    char *tmp = (char *) malloc(sizeof(char) * COMMAND_LEN);;
    getline(&tmp, &len, fp);
    strncpy(buf, tmp, COMMAND_LEN);
    free(tmp);
    fseek(fp, ++offset, SEEK_SET);
    return len;
}


size_t read_hist_line(struct HIST_FILE *hf, char buf[COMMAND_LEN], int action)
{
    /*
     * Move file pointer to previous new line.
     * Increment by one to get next line.
     * Store next line in buffer.
     */

    long offset = ftell(hf->fp);
    offset--;

    if (action == HIST_UP) {
        if (hf->current_line == 0) {
            fseek(hf->fp, 0, SEEK_SET);
            return read_line_and_move_fp_back(hf->fp, offset, buf);
        }

        while (fgetc(hf->fp) != '\n')
            fseek(hf->fp, --offset, SEEK_SET);
        hf->current_line--;

    } else if (action == HIST_DOWN) {
        /* if at the end of file, do not read any lines */
        if (hf->current_line == hf->len - 1) {
            buf[0] = 0;
            return 0;
        }

        /* edge case. if current_line == 0, go straight to read line */
        if (hf->current_line != 0) {
            while (fgetc(hf->fp) != '\n')
                fseek(hf->fp, ++offset, SEEK_SET);
        }
        hf->current_line++;
    }

    return read_line_and_move_fp_back(hf->fp, offset, buf);
}
