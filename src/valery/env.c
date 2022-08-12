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
#include <string.h>

#include "valery/env.h"
#include "builtins/builtins.h"
#include "lib/vstring.h"


static struct env_vars_t *env_vars_malloc(void)
{
    struct env_vars_t *env_vars = (struct env_vars_t *) malloc(sizeof(struct env_vars_t));
    env_vars->ht = ht_malloc();
    env_vars->update = false;
    env_vars->capacity = HT_TABLE_SIZE;
    env_vars->size = 0;

    env_vars->environ = (char **) malloc(env_vars->capacity * sizeof(char *));
    for (int i = 0; i < env_vars->capacity; i++)
        env_vars->environ[i] = (char *) malloc(MAX_ENV_LEN * sizeof(char));

    return env_vars;
}

static void env_vars_free(struct env_vars_t *env_vars)
{
    ht_free(env_vars->ht);

    for (int i = 0; i < env_vars->capacity; i++)
        free(env_vars->environ[i]);

    free(env_vars->environ);
    free(env_vars);
}

static struct paths_t *paths_malloc(void)
{
    struct paths_t *p = (struct paths_t *) malloc(sizeof(struct paths_t));
    p->paths = (char **) malloc(STARTING_PATHS * sizeof(char *));
    for (int i = 0; i < STARTING_PATHS; i++)
        p->paths[i] = (char *) malloc(MAX_ENV_LEN * sizeof(char));

    p->capacity = STARTING_PATHS;
    p->size = 0;
    return p;
}

static void paths_free(struct paths_t *p)
{
    for (int i = 0; i < p->capacity; i++)
        free(p->paths[i]);

    free(p->paths);
    free(p);
}

struct env_t *env_t_malloc(void)
{
    struct env_t *env = (env_t *) malloc(sizeof(env_t));
    env->env_vars = env_vars_malloc();
    env->paths = paths_malloc();
    env->aliases = ht_malloc();

    env->exit_code = 0;
    set_home_dir(env->env_vars);
    set_uid(env);

    return env;
}

void env_t_free(struct env_t *env)
{
    if (env == NULL)
        return;

    env_vars_free(env->env_vars);
    paths_free(env->paths);
    ht_free(env->aliases);
    free(env);
}

char *env_get(struct env_vars_t *env_vars, char *key)
{
    return (char *) ht_get(env_vars->ht, key);
}

struct ht_item_t *env_geth(struct env_vars_t *env_vars, unsigned int hash)
{
    return ht_geth(env_vars->ht, hash);
}

void env_set(struct env_vars_t *env_vars, char *key, char *value)
{
    ht_set(env_vars->ht, key, value);
    env_vars->update = true;
}

void env_rm(struct env_vars_t *env_vars, char *key)
{
    ht_rm(env_vars->ht, key);
    env_vars->update = true;
}

void env_gen(struct env_vars_t *env_vars, char *env_str[env_vars->capacity])
{
    int i;

    if (env_vars->update) {
        struct ht_item_t *item;
        i = 0;

        for (int hash = 0; hash < env_vars->capacity; hash++) {
            if (env_vars->ht->keys[hash] != 0) {
                item = env_geth(env_vars, hash);
                /* hash table may have collisions */
                while (item != NULL) {
                    snprintf(env_vars->environ[i], MAX_ENV_LEN, "%s=%s", item->key, (char *) item->value);
                    env_str[i] = env_vars->environ[i];
                    i++;
                    item = item->next;
                }
            }
        }

        env_str[i] = NULL;
        env_vars->size = --i;
        env_vars->update = false;
    } else {
        for (i = 0; i < env_vars->size; i++) {
            env_str[i] = env_vars->environ[i];
        }
        env_str[i] = NULL;
    }
}

void path_increase(struct paths_t *p, int new_len) {
    p->paths = (char **) realloc(p->paths, new_len * sizeof(char *));
    for (int i = p->capacity; i < new_len; i++)
        p->paths[i] = (char *) malloc(MAX_ENV_LEN * sizeof(char));

    p->capacity = new_len;
}

void env_update(struct env_t *env)
{
    env_update_pwd(env->env_vars);
    env_update_ps1(env);
}

void env_update_ps1(struct env_t *env)
{
    char default_ps1[] = ">";
    char *ps1 = env_get(env->env_vars, "PS1");
    if (ps1 == NULL)
        strncpy(env->ps1, default_ps1, 1024);

    /* parse PS1 */
    char c;
    char ps1_tmp[1024];
    int pos = 0;
    bool escape = false;
    while ((c = *ps1++) != 0) {
        if (c == '\\') {
            escape = true;
            continue;
        }
        if (!escape) {
            ps1_tmp[pos++] = c;
            continue;
        }

        switch (c) {
            char *home;
            char *cw;
            char *res;
            uid_t uid;
            case '$':
                uid = env->uid;
                if (uid == UID_ROOT)
                    ps1_tmp[pos++] = SYM_ROOT;
                else
                    ps1_tmp[pos++] = SYM_USR;
                break;

            case 'D':
                home = env_get(env->env_vars, "HOME");
                cw = env_get(env->env_vars, "PWD");

                if (cw == NULL)
                    break;
                if (home != NULL) {
                    /* try to replace HOME with ~ */
                    res = vstr_starts_with(cw, home);
                    if (res != NULL) {
                        ps1_tmp[pos++] = '~';
                        ps1_tmp[pos++] = '/';
                        /* move cw to end of HOME */
                        cw = res;
                    }
                }

                while ((c = *cw++) != 0)
                    ps1_tmp[pos++] = c;
                break;

            case 'C':
                strncat(ps1_tmp, "\033[0;36m", 8);
                pos += 7;
                break;

            case 'E':
                strncat(ps1_tmp, "\033[0m", 5);
                pos += 4;
                break;
        }

        escape = false;
    }

    ps1_tmp[pos] = 0;
    strncpy(env->ps1, ps1_tmp, 1024);
}

void env_update_pwd(struct env_vars_t *env_vars)
{
    char *old = env_get(env_vars, "PWD");
    char result[4096];
    if (pwd(result) == 1)
        return;

    /* update OLDPWD first, because pointer to old will be changed */
    if (old != NULL)
        env_set(env_vars, "OLDPWD", old);
    else
        env_set(env_vars, "OLDPWD", result);

    env_set(env_vars, "PWD", result);

    //TODO: check if actually need to update
    //env->env_vars->update = true;
}


int set_home_dir(struct env_vars_t *env_vars)
{
    struct passwd *pw = getpwuid(getuid());
    char *homedir = pw->pw_dir;
    
    if (homedir == NULL)
        return 1;
    
    env_set(env_vars, "HOME", homedir);
    return 0;
}

void set_uid(struct env_t *env)
{
    env->uid = getuid();
    char uid_tmp[32];
    snprintf(uid_tmp, 32, "%d", (unsigned int) env->uid);
    env_set(env->env_vars, "UID", uid_tmp);
}
