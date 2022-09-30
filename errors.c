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
#include <stdlib.h>
#include <stdio.h>

#include "vcommon.h"


void valery_exit(int exit_code)
{
    exit(exit_code);
}

void valery_exit_parse_error(const char *msg)
{
    fprintf(stderr, "valery: parse error: %s \n", msg);
    exit(1);
}

void valery_exit_internal_error(const char *file, const char *func, const int line)
{
    fprintf(stderr, "valery: internal error caused by line '%d' in function '%s' in file '%s'.\n",
            line, func, file);
    exit(1);
}

void valery_runtime_error(const char *msg, const char *file, const char *func, const int line)
{
    fprintf(stderr, "valery: runtime error: %s.", msg);
#ifdef DEBUG
    fprintf(stderr, " Caused by line '%d' in function '%s' in file '%s'.", line, func, file);
#endif /* DEBUG */
    putchar('\n');
}

void valery_error(const char *msg, const char *file, const char *func, const int line)
{
    fprintf(stderr, "valery: error: %s.", msg);
#ifdef DEBUG
    fprintf(stderr, " Caused by line '%d' in function '%s' in file '%s'.", line, func, file);
#endif /* DEBUG */
    putchar('\n');
}
