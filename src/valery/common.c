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

#include "valery/valery.h"


void valery_exit(int exit_code)
{
    exit(exit_code);
}

void valery_exit_parse_error(const char *msg)
{
    fprintf(stderr, "valery: parse error: %s \n", msg);
    exit(1);
}

void valery_exit_lex_error(const char *msg)
{
    fprintf(stderr, "valery: parse error: %s \n", msg);
    exit(1);
}

void _valery_exit_internal_error(char *msg, const char *file, const char *func, const int line)
{
    fprintf(stderr, "valery: internal error %s caused by line '%d' in function '%s' in file '%s'.\n",
            msg, line, func, file);
    exit(1);
}

void _valery_runtime_error(const char *msg, const char *file, const char *func, const int line)
{
    fprintf(stderr, "valery: runtime error: %s.", msg);
#ifdef DEBUG
    fprintf(stderr, " Caused by line '%d' in function '%s' in file '%s'.", line, func, file);
#endif /* DEBUG */
    putchar('\n');
}

void _valery_error(const char *msg, const char *file, const char *func, const int line)
{
    fprintf(stderr, "valery: error: %s.", msg);
#ifdef DEBUG
    fprintf(stderr, " Caused by line '%d' in function '%s' in file '%s'.", line, func, file);
#endif /* DEBUG */
    putchar('\n');
}

void *vmalloc(size_t size)
{
#ifdef VALLOC_IMPLEMENTATION
    void *tmp = malloc(size);
    if (tmp == NULL)
        valery_exit_internal_error("memory allocation error");
    return tmp;
#endif /* VALLOC_IMPLEMENTATION*/
    return malloc(size);
}

void *vcalloc(size_t nitems, size_t size)
{
#ifdef VALLOC_IMPLEMENTATION
    void *tmp = calloc(nitems, size);
    if (tmp == NULL)
        valery_exit_internal_error("memory allocation error");
    return tmp;
#endif /* VALLOC_IMPLEMENTATION*/
    return calloc(nitems, size);
}

void *vrealloc(void *ptr, size_t size)
{
#ifdef VALLOC_IMPLEMENTATION
    void *tmp = realloc(ptr, size);
    if (tmp == NULL)
        valery_exit_internal_error("memory allocation error");
    return tmp;
#endif /* VALLOC_IMPLEMENTATION*/
    return realloc(ptr, size);
}
