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

#include <stdint.h>

#ifndef VALERY
#define VALERY


/* variables */
#define COMMAND_LEN 1024
#define MAX_ENV_LEN 1024
#define CONFIG_NAME ".valeryrc"
#define HISTFILE_NAME ".valery_hist"
#define TOTAL_OPERANDS 5

extern const char *operands[TOTAL_OPERANDS];

/* types */
typedef struct ENV {
    uint8_t exit_code;
    char *PS1;
    char *PATH;
    char *HOME;
} ENV;

/* functions */
void disable_term_flags();
void enable_term_flags();
void catch_exit_signal(int signal);
struct ENV *malloc_env();
void free_env(struct ENV *env);

#endif
