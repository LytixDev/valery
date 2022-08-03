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

#ifndef VALERY
#define VALERY

#include <stdint.h>


/* variables */
#define COMMAND_LEN 1024

/* macros */
#ifdef DEBUG
#define print_debug(...) \
    do { printf("\033[0;31mDEBUG:\n"); fprintf(stderr, __VA_ARGS__); \
        printf("\033[0m\n"); } while (0);
#else
#define print_debug(...) ((void) 0)
#endif

/* functions */
void disable_term_flags();

void enable_term_flags();

#endif
