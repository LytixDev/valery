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

/* returns a pointer to allocated memory for the corresponding value to the given key */
char *env_get(struct env_t *env, char *key);

/*
 * returns a pointer to the first allocated ht_item_t corresponding to the given key.
 * note: there may be multiple items allocated under the same hash.
 */
struct ht_item_t *env_geth(struct env_t *env, unsigned int hash);

/*
 * generates the environment variables needed when executing a program on the form:
 * "KEY=VALUE". This is stored in env->environ. 
 * The given env_str parameter is populated with pointers to the all entries of 
 * env->environ. Last entry in env_str is set to NULL.
 */
void env_gen(struct env_t *env, char *env_str[env->env_capacity]);

/* calls ht_rm() */
void env_rm(struct env_t *env, char *key);

/* calls ht_set() */
void env_set(struct env_t *env, char *key, char *value);

/*
 * updates environment variables PWD and OLDPWD.
 * sets OLDPWD to PWD, and then updates PWD to the current working directory.
 * OLDPWD is set to the current working directory if PWD was NULL.
 */
void env_update_pwd(struct env_t *env);

#endif
