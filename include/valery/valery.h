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
#ifndef VALERY_H
#define VALERY_H

#include <stdlib.h>

/* variables */
#define COMMAND_LEN 1024
#define CONFIG_NAME ".valeryrc"
#define HISTFILE_NAME ".valery_hist"

#ifdef DEBUG_VERBOSE
#       define DEBUG_ENV
#       define DEBUG_INTERPRETER
#       define DEBUG_PROMPT
#endif

/* macros */
#ifdef DEBUG
#       define print_debug(...) \
                do { fprintf(stderr, "\033[0;31m"); fprintf(stderr, __VA_ARGS__); \
                     fprintf(stderr, "\033[0m\n"); } while (0);
#else
#       define print_debug(...) ((void) 0)
#endif

/* functions */
void valery_exit(int exit_code);

void valery_exit_parse_error(const char *msg);

void _valery_exit_internal_error(char *msg, const char *file, const char *func, const int line);
#define valery_exit_internal_error(m) _valery_exit_internal_error(m, __FILE__, __func__, __LINE__)

void _valery_runtime_error(const char *msg, const char *file, const char *func, const int line);
#define valery_runtime_error(m) _valery_runtime_error(m, __FILE__, __func__, __LINE__)

void _valery_error(const char *msg, const char *file, const char *func, const int line);
#define valery_error(m) _valery_error(m, __FILE__, __func__, __LINE__)

#ifndef VMALLOC_IMPLEMENTATION
#       define VMALLOC_IMPLEMENTATION
#endif
void *vmalloc(size_t size);


#endif /* VALERY_H */
