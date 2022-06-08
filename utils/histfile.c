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


struct HISTORY *init_history()
{
    struct HISTORY *hist = (HISTORY *) malloc(sizeof(HISTORY));
    hist->fp = (FILE *) malloc(sizeof(FILE));
    hist->f_current_line = 0;
    hist->f_len = 0;

    hist->stored_commands = malloc(MAX_COMMANDS_BEFORE_WRITE * sizeof(char *));
    /* allocate space for all strings */
    for (int i = 0; i < MAX_COMMANDS_BEFORE_WRITE; i++)
        hist->stored_commands[i] = malloc(COMMAND_LEN * sizeof(char));

    hist->total_stored_commands = 0;
    return hist;
}

void free_history(struct HISTORY *hist)
{
    if (hist == NULL)
        return;

    fclose(hist->fp);
    for (int i = 0; i < MAX_COMMANDS_BEFORE_WRITE; i++)
        free(hist->stored_commands[i]);
    free(hist->stored_commands);
    free(hist);
}

void save_command(struct HISTORY *hist, char buf[COMMAND_LEN])
{
    if (hist->total_stored_commands == MAX_COMMANDS_BEFORE_WRITE)
        write_commands_to_hist_file(hist);
    strncpy(hist->stored_commands[hist->total_stored_commands++], buf, COMMAND_LEN);
}

void write_commands_to_hist_file(struct HISTORY *hist)
{
    for (int i = 0; i < hist->total_stored_commands; i++) {
        if (strcmp(hist->stored_commands[i], "") != 0) {
            fputs(hist->stored_commands[i], hist->fp);
            fputs("\n", hist->fp);
            hist->f_len++;
        }
    }
    /* reset counters */
    hist->total_stored_commands = 0;
    hist->f_current_line = hist->f_len - 1;
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

/* returns 0 if it can open hist file, else 1 */
int open_hist_file(struct HISTORY *hist, char *full_path)
{
    hist->fp = fopen(full_path, "a+");
    if (hist->fp != NULL) {
        /* set current line to read from to last line of file (0 indexed) */
        hist->f_len = get_len(hist->fp);
        hist->f_current_line = hist->f_len - 1;
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

int update_current_line(struct HISTORY *hist, int action)
{
    action == HIST_UP ? hist->current_line-- : hist->current_line++;
    /* clamp on boundaries and return 1 which signals to not read the current line */
    if (action == HIST_UP && hist->current_line > 0) {
        hist->current_line = 0;
        return 1;
    }

    if (action == HIST_DOWN && hist->current_line > hist->f_len + hist->total_stored_commands) {
        hist->current_line = hist->f_len + hist->total_stored_commands;
        return 1;
    }
    
    return 0;
}

size_t read_hist_line(struct HISTORY *hist, char buf[COMMAND_LEN], int action)
{
    long offset = ftell(hist->fp);
    offset--;

    if (action == HIST_UP) {
        if (hist->f_current_line == 0) {
            fseek(hist->fp, 0, SEEK_SET);
            return read_line_and_move_fp_back(hist->fp, offset, buf);
        }

        while (fgetc(hist->fp) != '\n')
            fseek(hist->fp, --offset, SEEK_SET);
        hist->f_current_line--;

    } else if (action == HIST_DOWN) {
        /* if at the end of file, do not read any lines */
        if (hist->f_current_line == hist->f_len - 1) {
            buf[0] = 0;
            return 0;
        }

        /* edge case. if f_current_line == 0, go straight to read line */
        if (hist->f_current_line != 0) {
            while (fgetc(hist->fp) != '\n')
                fseek(hist->fp, ++offset, SEEK_SET);
        }
        hist->f_current_line++;
    }

    return read_line_and_move_fp_back(hist->fp, offset, buf);
}


size_t get_hist_line(struct HISTORY *hist, char buf[COMMAND_LEN], int action)
{
    int rc;
    printf("a:%d c:%ld, f:%ld\n", action, hist->current_line, hist->f_current_line);
    rc = update_current_line(hist, action);
    if (rc)
        return 0;

    /* read from hist file only when command is not recent enough to be in memory */
    //if (hist->current_line > hist->total_stored_commands)
    return read_hist_line(hist, buf, action);

    strncpy(buf, hist->stored_commands[hist->current_line], COMMAND_LEN);
    return strlen(buf);
}
