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

int read_current_line(struct HIST_FILE *hf, char *buf[COMMAND_LEN])
{
    /*
     * Move file pointer to previous new line.
     * Increment by one to get next line.
     * Store next line in buffer.
     */

    /* TODO: handle changing lines with arrow keys */

    if (hf->current_line == 0)
        return 1;

    size_t len = 0;
    long offset = ftell(hf->fp);
    offset--;

    while (fgetc(hf->fp) != '\n')
        fseek(hf->fp, --offset, SEEK_SET);

    getline(buf, &len, hf->fp);
    return 0;
}
/*
int main()
{
    struct HIST_FILE *hf = new_hist_file();

    if (open_hist_file(hf, "/home/nic/.valery_hist")) {
        printf("Could not open\n");
        return 1;
    }

    printf("Succesfully opened hist file\n\n");
    printf("pos before seek: %ld\n", ftell(hf->fp));

    char *buf[COMMAND_LEN];
    read_current_line(hf, buf);
    printf("pos after seek: %ld\n", ftell(hf->fp));
    printf("%s\n", *buf);


    TODO: keep track of all commands typed in, and write to
          hist file when program is closed.

}
*/
