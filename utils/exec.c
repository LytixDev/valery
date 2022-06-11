/*
 *  Forks the process and attempts to execute a program given as input.
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
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "sys/wait.h"
#include "exec.h"


int valery_exec(char *path, char *args)
{
    int status;
    int return_code = 0;
    char *args_cpy = args;
    /* env unused in this case */
    char *env[] = {NULL, NULL, NULL};
    /* args being an empty string results in undefined behavior */
    if (strcmp(args_cpy, "") == 0)
        args_cpy = NULL;

    char *full[] = {path, args_cpy};

    pid_t new_pid = fork();
    if (new_pid == CHILD_PID) {
        return_code = execve(path, full, env);
        return_code == -1 ? exit(EXIT_FAILURE) : exit(EXIT_SUCCESS);
    }

    waitpid(new_pid, &status, 0);

    return status != 0;
}
