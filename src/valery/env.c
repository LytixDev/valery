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

#include "valery/env.h"
#include "lib/hashtable.h"


struct env_t *env_t_malloc()
{
    struct env_t *env = (env_t *) malloc(sizeof(env_t));
    env->paths = (char **) malloc(STARTING_PATHS * sizeof(char *));
    for (int i = 0; i < STARTING_PATHS; i++)
        env->paths[i] = (char *) malloc(MAX_ENV_LEN * sizeof(char));

    env->path_capacity = STARTING_PATHS;
    env->path_size = 0;
    env->PATH = (char *) malloc(MAX_ENV_LEN * sizeof(char));
    env->exit_code = 0;

    env->env_vars = ht_malloc();

    //TODO: deprecate this
    env->PS1 = (char *) malloc(MAX_ENV_LEN * sizeof(char));
    env->HOME = (char *) malloc(MAX_ENV_LEN * sizeof(char));
    return env;
}

void env_t_free(struct env_t *env)
{
    if (env == NULL)
        return;
    
    for (int i = 0; i < env->path_capacity; i++)
        free(env->paths[i]);

    free(env->paths);
    free(env->PATH);

    ht_free(env->env_vars);

    //TODO: deprecate this
    free(env->PS1);
    free(env->HOME);

    free(env);
}

char *env_get(struct env_t *env, char *key)
{
    return ht_get(env->env_vars, key);
}

void env_set(struct env_t *env, char *key, char *value)
{
    ht_set(env->env_vars, key, value);
}


void env_t_path_increase(struct env_t *env, int new_len) {
    env->paths = (char **) realloc(env->paths, new_len * sizeof(char *));
    for (int i = env->path_capacity; i < new_len; i++)
        env->paths[i] = (char *) malloc(MAX_ENV_LEN * sizeof(char));

    env->path_capacity = new_len;
    return;
}
