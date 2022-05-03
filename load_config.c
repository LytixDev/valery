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
#include "valery.h"


int _get_config_path(char config_path[255])
{
    /* get home dir */
    struct passwd *pw = getpwuid(getuid());
    char *homedir = pw->pw_dir;
    
    if (homedir == NULL)
        return 1;

    strcpy(config_path, homedir);
    strcat(config_path, "/");
    strcat(config_path, CONFIG_NAME);

    return 0;
}

int _find_pos(char look_for, char *str)
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
    char config_path[255];
    size_t buf_len = 255;
    char buf[buf_len];
    char key[buf_len];
    char val[buf_len];
    int found_pos;

    int rc = _get_config_path(config_path);
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

            if (strcmp(key, "PS1") == 0) {
                strcpy(env->PS1, val);
            }

            if (strcmp(key, "PATH") == 0) {
                strcpy(env->PATH, val);
            }
        }

    }

    return 0;

}

/*
int main()
{
    struct ENV *env = (ENV *) malloc(sizeof(ENV));
    env->PS1 = (char *) malloc(sizeof(char));
    env->PATH = (char *) malloc(sizeof(char));
    int rc = parse_config(env);
    if (rc == 1) {
        free(env);
        return 1;
    }
    
    printf("%s\n", env->PS1);
    printf("%s\n", env->PATH);
    free(env);
}
*/
