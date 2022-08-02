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

#include <stdbool.h>

#include "lib/hashtable.h"


/* variables */
#define MAX_ENV_LEN 1024
#define CONFIG_NAME ".valeryrc"
#define HISTFILE_NAME ".valery_hist"
#define STARTING_PATHS 5


/* types */
typedef struct env_var_t {
    char *name;
    char *val;
} env_var_t;


typedef struct env_t {
    char *PATH; /* colon seperated string of all paths */
    char **paths;
    int path_size;
    int path_capacity;
    int exit_code;

    struct ht_t *env_vars;
    char **environ;     /* list of environment variables on the form: ["KEY=VALUE", ... ] */
    bool env_update;    /* set to true if a env_var has changed, and environ is not updated */
    int env_size;
    int env_capacity;
} env_t;


/* functions */
struct env_t *env_t_malloc();

void env_t_free(struct env_t *env);

void env_t_path_increase(struct env_t *env, int new_len);

char *env_get(struct env_t *env, char *key);

void env_set(struct env_t *env, char *key, char *value);

#endif
