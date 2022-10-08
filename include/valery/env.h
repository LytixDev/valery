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
#include <sys/types.h>

#include "lib/nicc/nicc.h"      // hashtable implementation


/* variables */
#define UID_ROOT 0
#define SYM_ROOT '#'
#define SYM_USR '$'
#define MAX_ENV_LEN 4096
#define STARTING_PATHS 5
#define ENV_HT_SIZE 64
#define ALIASES_HT_SIZE 32


/* types */
struct env_vars_t {
    struct ht_t *ht;    /* the hashtable that stores the environment variables */
    char **environ;     /* list of environment variables on the form: ["KEY=VALUE", ... ] */
    int size;
    int capacity;
    bool update;        /* set to true if an environment variable has changed, and environ is not outdated */
};


struct paths_t {
    char **paths;
    int size;
    int capacity;
};


struct env_t {
    struct env_vars_t *env_vars;
    struct paths_t *paths;  /* unwrapped PATH environment variable */
    struct ht_t *aliases;

    char ps1[MAX_ENV_LEN];
    uid_t uid;
};


/* functions */
struct env_t *env_malloc(void);

void env_free(struct env_t *env);

void env_update(struct env_t *env);

/* returns a pointer to allocated memory for the corresponding value to the given key */
char *env_get(struct env_vars_t *env_vars, char *key);

/*
 * returns a pointer to the first allocated ht_item_t corresponding to the given key.
 * note: there may be multiple items allocated under the same hash.
 */
struct ht_item_t *env_geth(struct env_vars_t *env_vars, unsigned int hash);

/* calls ht_rm() */
void env_rm(struct env_vars_t *env_vars, char *key);

/* calls ht_set() */
void env_set(struct env_vars_t *env_vars, char *key, char *value);

/*
 * generates the environment variables needed when executing a program on the form:
 * "KEY=VALUE". This is stored in env->environ.
 * The given env_str argument is populated with pointers to the all entries of
 * env->environ. Last entry in env_str is set to NULL.
 */
void env_gen(struct env_vars_t *env_vars, char *env_str[env_vars->capacity]);

struct env_t *init_env(void);

#endif
