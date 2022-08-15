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

#include <stdio.h>
#include <string.h>

#include "valery.h"
#include "valery/env.h"
#include "valery/load_config.h"
#include "lib/vstring.h"


int parse_config(struct env_vars_t *env_vars, struct paths_t *p)
{
    /*
     * TODO: look for config in various places, f.ex: ~/.config/ 
     * NOTE: parsing the config should ideally be done identically to how
     * input buffers given from te prompt are parsed, but at this point,
     * that whole event loop is not robust enough to handle that.
     */

    FILE *fp;
    char config_path[MAX_ENV_LEN];
    char buf[MAX_ENV_LEN];
    char key[MAX_ENV_LEN];
    char val[MAX_ENV_LEN];
    int found_pos;
    int rc;

    rc = get_config_path(config_path, env_get(env_vars, "HOME"));
    if (rc == 1)
        return 1;

    fp = fopen(config_path, "r");
    if (fp == NULL)
        return 1;

    while (fgets(buf, MAX_ENV_LEN, fp)) {
        /* parse line */
        if (buf[0] == '#')
            continue;

        found_pos = vstr_find_first_c(buf, '=');
        if (found_pos != -1) {
            /*TODO: improve this */
            unsigned long str_len = strlen(buf);
            strncpy(key, buf, found_pos);
            strncpy(val, buf + found_pos + 1, str_len);
            key[found_pos] = '\0';
            val[str_len - found_pos - 2] = '\0';

            env_set(env_vars, key, val);
        }

    }

    fclose(fp);
    unwrap_paths(p, (char *) env_get(env_vars, "PATH"));
    return 0;
}

int get_config_path(char result[MAX_ENV_LEN], char *HOME)
{
    if (HOME != NULL) {
        snprintf(result, MAX_ENV_LEN, "%s/%s", HOME, CONFIG_NAME);
        return 0;
    } else {
        fprintf(stderr, "VALERY ERROR: could not find HOME environment variable\n");
        return 1;
    }
}

void unwrap_paths(struct paths_t *p, char *PATHS)
{
    if (PATHS == NULL) {
        fprintf(stderr, "valery: no PATH variable found in .valeryrc\n");
        return;
    }

    const char delim[] = ":";
    char *path = strtok(PATHS, delim);
    
    while (path != NULL) {
        if (p->size == p->capacity - 1)
            path_increase(p, p->capacity * 2);
       
        strncpy(p->paths[p->size++], path, MAX_ENV_LEN);
        path = strtok(NULL, delim);
    }
}
