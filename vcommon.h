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

#ifndef VCOMMON_H
#define VCOMMON_H

#include <stdint.h>


/* macros */

#ifdef DEBUG
#       define print_debug(...) \
                do { fprintf(stderr, "\033[0;31mDEBUG:\n"); fprintf(stderr, __VA_ARGS__); \
                     fprintf(stderr, "\033[0m\n"); } while (0);
#else
#       define print_debug(...) ((void) 0)
#endif


/* functions */

void valery_exit(int exit_code);

void valery_exit_parse_error(const char *msg);

void valery_exit_internal_error(const char *file, const char *func, const int line);
#define internal_error_exit() valery_exit_internal_error(__FILE__, __func__, __LINE__)

void valery_runtime_error(const char *msg, const char *file, const char *func, const int line);
#define runtime_error(m) valery_runtime_error(m, __FILE__, __func__, __LINE__)


void valery_error(const char *msg, const char *file, const char *func, const int line);
#define verror(m) valery_error(m, __FILE__, __func__, __LINE__)


#endif /* VCOMMON_H */
