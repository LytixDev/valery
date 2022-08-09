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
#include <unistd.h>
#include <pwd.h>
#include <sys/types.h>

#include "valery/env.h"
#include "lib/hashtable.h"
#include "builtins/builtins.h"
#include "valery/load_config.h"


struct env_t *env_t_malloc()
{
    struct env_t *env = (env_t *) malloc(sizeof(env_t));
    env->exit_code = 0;

    env->paths = (char **) malloc(STARTING_PATHS * sizeof(char *));
    for (int i = 0; i < STARTING_PATHS; i++)
        env->paths[i] = (char *) malloc(MAX_ENV_LEN * sizeof(char));

    env->path_capacity = STARTING_PATHS;
    env->path_size = 0;
    env->PATH = (char *) malloc(MAX_ENV_LEN * sizeof(char));

    env->env_vars = ht_malloc();
    env->env_update = false;
    env->env_capacity = HT_TABLE_SIZE;
    env->env_size = 0;
    env->environ = (char **) malloc(env->env_capacity * sizeof(char *));
    for (int i = 0; i < env->env_capacity; i++)
        env->environ[i] = (char *) malloc(MAX_ENV_LEN * sizeof(char));

    set_home_dir(env);

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

    for (int i = 0; i < env->env_capacity; i++)
        free(env->environ[i]);

    free(env->environ);
    ht_free(env->env_vars);
    free(env);
}

char *env_get(struct env_t *env, char *key)
{
    return (char *) ht_get(env->env_vars, key);
}

struct ht_item_t *env_geth(struct env_t *env, unsigned int hash)
{
    return ht_geth(env->env_vars, hash);
}

void env_set(struct env_t *env, char *key, char *value)
{
    ht_set(env->env_vars, key, value);
    env->env_update = true;
}

void env_rm(struct env_t *env, char *key)
{
    ht_rm(env->env_vars, key);
    env->env_update = true;
}

void env_gen(struct env_t *env, char *env_str[env->env_capacity])
{
    int i;

    if (env->env_update) {
        struct ht_item_t *item;
        i = 0;

        for (int hash = 0; hash < env->env_capacity; hash++) {
            if (env->env_vars->keys[hash] != 0) {
                item = env_geth(env, hash);
                /* hash table may have collisions */
                while (item != NULL) {
                    snprintf(env->environ[i], MAX_ENV_LEN, "%s=%s", item->key, (char *) item->value);
                    env_str[i] = env->environ[i];
                    i++;
                    item = item->next;
                }
            }
        }

        env_str[i] = NULL;
        env->env_size = --i;
        env->env_update = false;
    } else {
        for (i = 0; i < env->env_size; i++) {
            env_str[i] = env->environ[i];
        }
        env_str[i] = NULL;
    }
}

void env_t_path_increase(struct env_t *env, int new_len) {
    env->paths = (char **) realloc(env->paths, new_len * sizeof(char *));
    for (int i = env->path_capacity; i < new_len; i++)
        env->paths[i] = (char *) malloc(MAX_ENV_LEN * sizeof(char));

    env->path_capacity = new_len;
    return;
}

void env_update_pwd(struct env_t *env)
{
    char *old = env_get(env, "PWD");
    char result[4096];
    if (pwd(result) == 1)
        return;

    /* update OLDPWD first, because pointer to old will be changed */
    if (old != NULL)
        env_set(env, "OLDPWD", old);
    else
        env_set(env, "OLDPWD", result);

    env_set(env, "PWD", result);

    //TODO: check if actually need to update
    env->env_update = true;
}


int set_home_dir(struct env_t *env)
{
    struct passwd *pw = getpwuid(getuid());
    char *homedir = pw->pw_dir;
    
    if (homedir == NULL)
        return 1;
    
    env_set(env, "HOME", homedir);
    return 0;
}
