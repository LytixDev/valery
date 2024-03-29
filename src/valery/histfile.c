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

#include "valery/valery.h"
#include "valery/histfile.h"


/* returns 1 if the given action will put the file pointer out of
 * bounds, else 0.
 */
static int out_of_bounds(struct hist_t *hist, enum histaction_t action)
{
    if (action == HIST_UP && hist->pos == 0)
        return 1;

    /* minus 1 because hist->pos is zero indexed */
    if (action == HIST_DOWN && hist->pos == hist->s_len + hist->f_len - 1)
        return 1;

    return 0;
}

/* 
 * stores the amount of lines in the file and in the process moves
 * the file pointer to the end of the file.
 */
static void hist_count(struct hist_t *hist)
{
    size_t len = 0;
    long chars = 0;

    while (!feof(hist->fp)) {
        chars++;
        if (fgetc(hist->fp) == '\n')
            len++;
    }

    hist->f_len = len;
    hist->f_chars = chars;
}

/*
 * opens a read and write connection to the hist file.
 * creates a new hist file if input path does not exist.
 * returns 1 if no connection could be made, else 0.
 */
static int hist_open(struct hist_t *hist, char *path)
{
    hist->fp = fopen(path, "a+");

    if (hist->fp != NULL) {
        /* hist_t_count() moves file pointer to end of file */
        hist_count(hist);
        hist->f_pos = hist->f_len;
        return 0;
    }

    return 1;
}

/*
 * traverses the hist one line in the given direction.
 * NB: function does not make sure direction does not cause
 * hist to go out of bounds.
 */
long hist_traverse(struct hist_t *hist, enum histaction_t direction)
{
    direction == HIST_UP ? hist->pos-- : hist->pos++;

    /* histline in memory, do not update file pointer in histfile */
    if (hist->pos + 1 > hist->f_len)
        return READ_FROM_MEMORY;

    /* read from hist file */
    long position_in_file = ftell(hist->fp);
    /* decrement pos to one char before \n, unless it is at the beginning */
    if (position_in_file != 0) --position_in_file;

    while (fgetc(hist->fp) != '\n') {
        if (direction == HIST_UP) {
            if (position_in_file == 0) { 
                /* actual position will be delayed by one, so set file pointer to zero */
                fseek(hist->fp, 0, SEEK_SET);
                break;
            }
            fseek(hist->fp, --position_in_file, SEEK_SET);
        } else {
            if (position_in_file == hist->f_chars) break;
            fseek(hist->fp, ++position_in_file, SEEK_SET);
        }
    }
    position_in_file++;

    return position_in_file;
}

/*
 * reads one line of the hist file and moves the file pointer back to the given offset.
 * if offset is -1, it uses ftell(hist->fp) to get the offset
 */
static void hist_read_line_f(struct hist_t *hist, char buf[MAX_COMMAND_LEN], long offset)
{
    if (offset == -1)
        offset = ftell(hist->fp);
    /* read hist line into buffer */
    fgets(buf, MAX_COMMAND_LEN, hist->fp);
    /* move file pointer back to last '\n' */
    fseek(hist->fp, offset, SEEK_SET);
}

void hist_reset_pos(struct hist_t *hist)
{
    /* move file pointer to end of file */
    fseek(hist->fp, 0, SEEK_END);
    /* reset position counters */
    hist->f_pos = hist->f_len;
    hist->pos = hist->f_len + hist->s_len;
}

enum readfrom_t hist_get_line(struct hist_t *hist, char buf[MAX_COMMAND_LEN], enum histaction_t action)
{
    if (out_of_bounds(hist, action))
        return DID_NOT_READ;

    long rc = hist_traverse(hist, action);
    if (rc == READ_FROM_MEMORY) {
        size_t index = hist->pos - hist->f_len;
        strncpy(buf, hist->stored_commands[index], MAX_COMMAND_LEN);
        return READ_FROM_MEMORY;
    }

    /* read from hist file, rc is the offset of the file pointer */
    hist_read_line_f(hist, buf, rc);
    return READ_FROM_HIST;
}

void hist_write(struct hist_t *hist)
{
    for (size_t i = 0; i < hist->s_len; i++) {
        if (strcmp(hist->stored_commands[i], "") != 0) {
            fputs(hist->stored_commands[i], hist->fp);
            fputs("\n", hist->fp);
            hist->f_len++;
        }
    }
    /* move file pointer back to end of file */
    fseek(hist->fp, 0, SEEK_END);
}

void hist_save(struct hist_t *hist, char buf[MAX_COMMAND_LEN])
{
    if (hist->s_len == MAX_COMMANDS_BEFORE_WRITE)
        hist_write(hist);
    strncpy(hist->stored_commands[hist->s_len++], buf, MAX_COMMAND_LEN);
}

struct hist_t *hist_malloc(char *full_path_to_hist_file)
{
    struct hist_t *hist = (struct hist_t *) vmalloc(sizeof(struct hist_t));
    hist->s_len = 0;
    hist->stored_commands = vmalloc(MAX_COMMANDS_BEFORE_WRITE * sizeof(char *));

    /* allocate space for all strings */
    for (int i = 0; i < MAX_COMMANDS_BEFORE_WRITE; i++)
        hist->stored_commands[i] = vmalloc(MAX_COMMAND_LEN * sizeof(char));

    int rc = hist_open(hist, full_path_to_hist_file);
    /* if no connection could be made to the hist file, set the pointer to null */
    if (rc == 1)
        hist->fp = NULL;

    /*
     * initially no commands in memory, so set pos to file pos
     * NB: hist->pos is zero indexed
     */
    hist->pos = hist->f_pos - 1;
    
    return hist;
}

void hist_free(struct hist_t *hist)
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

struct hist_t *hist_init(char *home_folder)
{
    char hist_file_path[1024] = {0};
    snprintf(hist_file_path, 1024, "%s/%s", home_folder, HISTFILE_NAME);
    return hist_malloc(hist_file_path);
}
