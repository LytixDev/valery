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
        free(hf->fp);
        free(hf);
    }
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
    hf->fp = fopen(full_path, "r+");
    if (hf->fp != NULL) {
        /* set current line to read from to last line of file (0 indexed) */
        hf->len = get_len(hf->fp);
        hf->current_line = hf->len - 1;
        return 0;
    }

    return 1;
}

int read_line_and_move_fp_back(FILE *fp, long offset, char *buf[COMMAND_LEN])
{
    size_t len = 0;
    getline(buf, &len, fp);
    fseek(fp, ++offset, SEEK_SET);
    return len;
}


int read_hist_line(struct HIST_FILE *hf, char *buf[COMMAND_LEN], int action)
{
    /*
     * Move file pointer to previous new line.
     * Increment by one to get next line.
     * Store next line in buffer.
     */

    long offset = ftell(hf->fp);
    offset--;

    /* TODO: wrap on boundaries */
    if (action == HIST_UP) {
        if (hf->current_line == 0) {
            read_line_and_move_fp_back(hf->fp, offset, buf);
            return 0;
        }

        while (fgetc(hf->fp) != '\n')
            fseek(hf->fp, --offset, SEEK_SET);
        hf->current_line--;

    } else if (action == HIST_DOWN) {
        if (hf->current_line == hf->len - 1) {
            read_line_and_move_fp_back(hf->fp, offset, buf);
            return 0;
        }

        while (fgetc(hf->fp) != '\n')
            fseek(hf->fp, ++offset, SEEK_SET);
        hf->current_line++;
    }

    read_line_and_move_fp_back(hf->fp, offset, buf);
    return 0;
}
/*
int main()
{
    struct HIST_FILE *hf = new_hist_file();

    if (open_hist_file(hf, "/home/nic/.valery_hist")) {
        printf("Could not open\n");
        free_hist_file(hf);
        return 1;
    }


    char *buf[COMMAND_LEN];
    read_hist_line(hf, buf, HIST_UP);
    printf("last: %s\n", *buf);

    hf->current_line--;
    read_hist_line(hf, buf, HIST_UP);
    printf("up: %s\n", *buf);

    hf->current_line--;
    read_hist_line(hf, buf, HIST_UP);
    printf("up: %s\n", *buf);

    hf->current_line++;
    read_hist_line(hf, buf, HIST_DOWN);
    printf("down: %s\n", *buf);

    hf->current_line = hf->len - 1;
    read_hist_line(hf, buf, HIST_DOWN);
    printf("last: %s\n", *buf);

    //TODO: keep track of all commands typed in, and write to
    //      hist file when program is closed.
    
    free_hist_file(hf);
    return 0;
}
*/
