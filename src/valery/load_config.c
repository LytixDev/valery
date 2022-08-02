/*
 *  Reads .valeryrc and sets environmental variables  
 *   
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
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>
#include <string.h>
#include <stdlib.h>

#include "valery/load_config.h"
#include "valery/env.h"


int set_home_dir(struct env_t *env)
{
    struct passwd *pw = getpwuid(getuid());
    char *homedir = pw->pw_dir;
    
    if (homedir == NULL)
        return 1;
    
    env_set(env, "HOME", homedir);
    return 0;
}

int get_config_path(struct env_t *env, char config_path[MAX_ENV_LEN])
{
    char *HOME = env_get(env, "HOME");
    if (HOME != NULL) {
        snprintf(config_path, MAX_ENV_LEN, "%s/%s", HOME, CONFIG_NAME);
        return 0;
    } else {
        fprintf(stderr, "VALERY ERROR: could not find HOME environment variable\n");
        return 1;
    }
}

static int find_pos(char look_for, char *str)
{
    int found_pos = 0;

    while (str[found_pos] != '\0') {
        if (str[found_pos] == look_for)
            return found_pos;
        found_pos++;
    }

    return -1;
}

void unwrap_paths(struct env_t *env)
{
    const char delim[] = ":";
    char *path = strtok(env->PATH, delim);
    
    while (path != NULL) {
        if (env->path_size == env->path_capacity - 1)
            env_t_path_increase(env, env->path_capacity + 5);
       
        strcpy(env->paths[env->path_size++], path);
        path = strtok(NULL, delim);
    }
}

int parse_config(struct env_t *env)
{
    /*
     * TODO: look for config in various places, f.ex: ~/.config/ 
     * get home dir in a more robust fashion
     */

    FILE *fp;
    char config_path[MAX_ENV_LEN];
    char buf[MAX_ENV_LEN];
    char key[MAX_ENV_LEN];
    char val[MAX_ENV_LEN];
    int found_pos;
    int rc;

    rc = set_home_dir(env);
    if (rc == 1)
        return 1;

    rc = get_config_path(env, config_path);
    if (rc == 1)
        return 1;

    fp = fopen(config_path, "r");
    if (fp == NULL)
        return 1;

    while (fgets(buf, MAX_ENV_LEN, fp)) {
        /* parse line */
        if (buf[0] == '#')
            continue;

        found_pos = find_pos('=', buf);
        if (found_pos != -1) {
            /*TODO: improve this */
            unsigned long str_len = strlen(buf);
            strncpy(key, buf, found_pos);
            strncpy(val, buf + found_pos + 1, str_len);
            key[found_pos] = '\0';
            val[str_len - found_pos - 2] = '\0';

            if (strcmp(key, "PATH") == 0)
                strcpy(env->PATH, val);
            else
                env_set(env, key, val);
        }

    }
    fclose(fp);
    unwrap_paths(env);
    return 0;
}
