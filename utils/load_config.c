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

#include "load_config.h"
#include "../valery.h"

int set_home_dir(struct ENV *env)
{
    struct passwd *pw = getpwuid(getuid());
    char *homedir = pw->pw_dir;
    
    if (homedir == NULL)
        return 1;
    
    strncpy(env->HOME, homedir, MAX_ENV_LEN);
    return 0;
}

int get_config_path(struct ENV *env, char config_path[MAX_ENV_LEN])
{

    snprintf(config_path, MAX_ENV_LEN, "%s/%s", env->HOME, CONFIG_NAME);

    return 0;
}

static int _find_pos(char look_for, char *str)
{
    int found_pos = 0;

    while (str[found_pos] != '\0') {
        if (str[found_pos] == look_for)
            return found_pos;
        found_pos++;
    }

    return -1;
}

int parse_config(struct ENV *env)
{
    /*
     * TODO: look for config in various places, f.ex: ~/.config/ 
     * get home dir in a more robust fashion
     */

    FILE *fp;
    char config_path[MAX_ENV_LEN];
    size_t buf_len = MAX_ENV_LEN;
    char buf[buf_len];
    char key[buf_len];
    char val[buf_len];
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

    while (fgets(buf, buf_len, fp)) {
        /* parse line */
        if (buf[0] == '#')
            continue;

        found_pos = _find_pos('=', buf);
        if (found_pos != -1) {
            /*TODO: improve this */
            unsigned long str_len = strlen(buf);
            strncpy(key, buf, found_pos);
            strncpy(val, buf + found_pos + 1, str_len);
            key[found_pos] = '\0';
            val[str_len - found_pos - 2] = '\0';

            if (strcmp(key, "PS1") == 0)
                strcpy(env->PS1, val);

            if (strcmp(key, "PATH") == 0)
                strcpy(env->PATH, val);
        }

    }

    //TODO: fix this
    //#define  _POSIX_C_SOURCE = 200112L
    //char *p = getenv("HOME");
    //printf("%s\n", p);
    //setenv("TERM", "st", 1);
    //p = getenv("TERM");
    //printf("%s\n", p);


    return 0;

}
