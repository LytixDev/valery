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


struct HISTORY *init_history(char *full_path_to_hist_file)
{
    struct HISTORY *hist = (HISTORY *) malloc(sizeof(HISTORY));
    hist->fp = (FILE *) malloc(sizeof(FILE));
    hist->t_stored = 0;
    hist->stored_commands = malloc(MAX_COMMANDS_BEFORE_WRITE * sizeof(char *));

    /* allocate space for all strings */
    for (int i = 0; i < MAX_COMMANDS_BEFORE_WRITE; i++)
        hist->stored_commands[i] = malloc(COMMAND_LEN * sizeof(char));

    int rc = open_hist_file(hist, full_path_to_hist_file);
    /* if no connection could be made to the hist file, set the pointer to null */
    if (rc == 1)
        hist->fp = NULL;

    /* initially no commands in memory, so set pos to file pos */
    hist->pos = hist->f_pos;
    
    return hist;
}

void free_history(struct HISTORY *hist)
{
    if (hist == NULL)
        return;

    if (hist->fp != NULL)
        fclose(hist->fp);

    for (int i = 0; i < MAX_COMMANDS_BEFORE_WRITE; i++)
        free(hist->stored_commands[i]);

    free(hist->stored_commands);
    free(hist);
}

int out_of_bounds(struct HISTORY *hist, histaction_t action)
{
    if (action == HIST_UP && hist->pos == 0)
        return 1;

    if (action == HIST_DOWN && hist->pos == hist->t_stored + hist->f_len)
        return 1;

    return 0;
}

void save_command(struct HISTORY *hist, char buf[COMMAND_LEN])
{
    if (hist->t_stored == MAX_COMMANDS_BEFORE_WRITE)
        write_commands_to_hist_file(hist);
    strncpy(hist->stored_commands[hist->t_stored++], buf, COMMAND_LEN);
}

void write_commands_to_hist_file(struct HISTORY *hist)
{
    for (int i = 0; i < hist->t_stored; i++) {
        if (strcmp(hist->stored_commands[i], "") != 0) {
            fputs(hist->stored_commands[i], hist->fp);
            fputs("\n", hist->fp);
            hist->f_len++;
        }
    }
    /* move file pointer back to end of file */
    fseek(hist->fp, 0, SEEK_END);
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

int open_hist_file(struct HISTORY *hist, char *path)
{
    hist->fp = fopen(path, "a+");

    if (hist->fp != NULL) {
        /* get_len() moves file pointer to end of file */
        hist->f_len = get_len(hist->fp);
        hist->f_pos = hist->f_len;
        return 0;
    }

    return 1;
}

void read_line_and_move_fp_back(FILE *fp, long offset, char buf[COMMAND_LEN])
{
    size_t len = 0;
    char *tmp = (char *) malloc(sizeof(char) * COMMAND_LEN);;
    getline(&tmp, &len, fp);
    strncpy(buf, tmp, COMMAND_LEN);
    free(tmp);
    fseek(fp, ++offset, SEEK_SET);
}

void read_hist_line(struct HISTORY *hist, char buf[COMMAND_LEN], histaction_t action)
{
    /* get current file pointer position, and move one before the current line */
    long offset = hist->f_pos == 0 ? 1 : ftell(hist->fp);
    offset--;

    if (action == HIST_UP) {
        /* special case when file pointer is at the start of the file */
        if (hist->f_pos == hist->f_len) {
            fseek(hist->fp, 0, SEEK_END);
            offset = ftell(hist->fp);
            offset--;
        }
        
        /*
         * special case when pos is at the beginning of the file,
         * and there is no newline to seek after
         */
        if (hist->f_pos == 1) {
            fseek(hist->fp, 0, SEEK_SET);
        } else {
            while (fgetc(hist->fp) != '\n')
                fseek(hist->fp, --offset, SEEK_SET);
        }

        hist->f_pos--;
    } else {
        /* special case when file pointer is at the start of the file */
        if (hist->f_pos == 0) {
            fseek(hist->fp, 0, SEEK_SET);
        } else {
            while (fgetc(hist->fp) != '\n')
                fseek(hist->fp, ++offset, SEEK_SET);
            offset++;
        }

        hist->f_pos++;
    }

    read_line_and_move_fp_back(hist->fp, offset, buf);
}

void reset_hist_pos(struct HISTORY *hist)
{
    /* move file pointer to end of file */
    fseek(hist->fp, 0, SEEK_END);
    /* reset position counters */
    hist->f_pos = hist->f_len;
    hist->pos = hist->f_len + hist->t_stored;
}

readfrom_t get_hist_line(struct HISTORY *hist, char buf[COMMAND_LEN], int action)
{
    int rc;
    rc = out_of_bounds(hist, action);
    if (rc)
        return DID_NOT_READ;

    /* read from memory */
    if (hist->pos > hist->f_len) {
        int index = hist->pos - hist->f_len - 1;
        strncpy(buf, hist->stored_commands[index], COMMAND_LEN);
        return READ_FROM_MEMORY;
    }

    /* read from hist file */
    read_hist_line(hist, buf, action);
    return READ_FROM_HIST;
}
