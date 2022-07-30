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

#ifndef ENV
#define ENV


/* variables */
#define MAX_ENV_LEN 1024
#define CONFIG_NAME ".valeryrc"
#define HISTFILE_NAME ".valery_hist"
#define STARTING_PATHS 5


/* types */
typedef struct env_t {
    char *PATH;
    char **paths;
    int path_size;
    int path_capacity;
    int exit_code;
    /* environment variables */
    char *PS1;
    char *HOME;
} env_t;


/* functions */
struct env_t *env_t_malloc();

void env_t_free(struct env_t *env);

void env_t_path_increase(struct env_t *env, int new_len);

#endif
