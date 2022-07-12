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
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "builtins.h"


int which(char *program_name, char **paths, int total_paths, char **path_result)
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

    for (int i = 0; i < total_paths; i++) {
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
                if (stat(final, &sb) == 0 && sb.st_mode & S_IXUSR) {
                    closedir(d);

                    /* path_result is NULL means 'which' is used interactively,
                     * and should print the found path, but not attempt to 
                     * modify path_result as this is not used in this case */
                    if (path_result == NULL)
                        printf("%s/%s\n", path, program_name);
                    else
                        *path_result = path;

                    return COMMAND_IN_PATH;
                }
            }
        }
    }

    if (path_result == NULL)
        fprintf(stderr, "%s: not found\n", program_name);

    return COMMAND_NOT_FOUND;
}
/*
int main(int argc, char *argv[])
{
    if (argc == 3) {
        int rc = which(argv[1], argv[2]);
        return rc;
    }

    return 1;
}
*/
