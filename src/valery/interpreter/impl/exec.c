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
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "sys/wait.h"


int valery_exec_program(int argc, char *argv[])
{
    int status;
    int rc;
    int return_code;

    char first_arg[1024];
    snprintf(first_arg, 1024, "/bin/%s", argv[0]);

    /*
     * full must contain program name and an argument.
     * last argument must be NULL to signify end of pointer arr.
     * ex: full = { "/bin/ls", "-la", NULL }
     */
    char *full[argc + 1];
    full[0] = first_arg;
    for (int i = 1; i < argc; i++)
        full[i] = argv[i];

    full[argc] = NULL;

    pid_t new_pid = fork();
    if (new_pid == 0)
        return_code = execve(first_arg, full, NULL);

    waitpid(new_pid, &status, 0);
    return status != 0;
}
