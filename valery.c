/*
 *  <one line to give the program's name and a brief idea of what it does.>
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
#include <stdint.h>

#include "valery.h"
#include "load_config.c"


void disp(uint8_t *ps1, char *ret)
{
    printf("%s ", ps1);
    fgets(ret, COMMAND_LEN, stdin);
    printf("input was: %s\n", ret);
}

struct ENV *new_env()
{
    struct ENV *env = (ENV *) malloc(sizeof(ENV));
    env->PS1 = (char *) malloc(sizeof(char));
    env->PATH = (char *) malloc(sizeof(char));
    return env;
}

void free_env(struct ENV *env)
{
    if (env != NULL) {
        free(env->PS1);
        free(env->PATH);
        free(env);
    }
}

int main()
{
    struct ENV *env = new_env();
    int rc = parse_config(env);

    if (rc == 1) {
        /* TODO: improve error handling */
        fprintf(stderr, "error parsing .valeryrc");
        free(env);
        return 1;
    }

    /* testing purposes */
    printf("Environment variables found from .valeryrc:\n");
    printf("PS1: %s\n", env->PS1);
    printf("PATH: %s\n", env->PATH);

    free(env);
    return 0;
}
