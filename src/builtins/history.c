/*
 *  Prints the 15 most recent typed in commands to stdout.
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
#include <stdbool.h>

#include "valery/histfile.h"

#define LINES 15


int history(struct hist_t *hist, bool print_all)
{
    if (hist->fp == NULL)
        return 1;

    char buf[COMMAND_LEN];
    long rc;
    size_t i;
    size_t len;
    size_t histlines = hist->s_len + hist->f_len;
    if (histlines == 0)
        return 1;

    hist_t_reset_pos(hist);

    if (print_all)
        len = histlines;
    else
        /* only read valid amount of histlines */
        len = histlines >= LINES ? LINES : histlines;

    for (i = len; i > 0; i--)
        hist_t_traverse(hist, HIST_UP); 

    for (i = len; i > 1; i--) {
        rc = hist_t_get_line(hist, buf, HIST_DOWN);
        /* chop off new line character */
        if (rc == READ_FROM_HIST)
            buf[strlen(buf) - 1] = 0;
        printf("%6ld %s\n", hist->pos, buf);
    }

    return 0;
}
