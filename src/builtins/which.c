/*
 *  Rewrite of 'which'. Shows the full path of shell commands in the PATH.
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

#include <dirent.h>
#include <string.h>
#include <stdio.h>
#include <sys/stat.h>

#include "builtins/builtins.h"


int which_single(char *program_name, char **paths, int path_count, char **path_result)
{
    /* check if program name is shell builtin */
    for (int i = 0; i < total_builtin_functions; i++) {
        if (strcmp(builtin_names[i], program_name) == 0) {
            if (path_result == NULL)
                printf("%s: shell builtin\n", program_name);

            return COMMAND_IS_BUILTIN;
        }
    }

    DIR *d;
    struct dirent *dir;
    struct stat sb;
    /* if program_name is already a path, do not add any PATH to it */
    bool program_is_path = program_name[0] == '/';
    if (program_is_path) {
        if (stat(program_name, &sb) == 0 && sb.st_mode & S_IXUSR && !S_ISDIR(sb.st_mode)) {
            if (path_result == NULL)
                printf("%s\n", program_name);
            return COMMAND_IS_PATH;
        } else {
            goto not_found;
        }
    }
    
    for (int i = 0; i < path_count; i++) {
        char *path = paths[i];
        d = opendir(path);
        if (d == NULL)
            continue;

        while ((dir = readdir(d)) != NULL) {
            /* check for name equality */
            if (strcmp(program_name, dir->d_name) == 0) {
                char final[1024];
                snprintf(final, 1024, "%s/%s", path, program_name);

                /* check if executable bit is on */
                if (stat(final, &sb) == 0 && sb.st_mode & S_IXUSR && !S_ISDIR(sb.st_mode)) {
                    closedir(d);

                    /* path_result is NULL means 'which' is used interactively,
                     * and should print the found path, but not attempt to 
                     * modify path_result as this is not used in this case */
                    if (path_result == NULL)
                        printf("%s\n", final);
                    else
                        *path_result = path;

                    return COMMAND_IN_PATH;
                }
            }
        }
        closedir(d);
    }

not_found:
    if (path_result == NULL)
        fprintf(stderr, "%s: not found\n", program_name);

    return COMMAND_NOT_FOUND;
}

int which(char **program_names, int program_count, char **paths, int path_count)
{
    int rc = 0;
    int rc_tmp;

    for (int i = 0; i < program_count; i++) {
        rc_tmp = which_single(program_names[i], paths, path_count, NULL);
        if (rc_tmp == COMMAND_NOT_FOUND)
            rc = 1;
    }
    return rc;
}
