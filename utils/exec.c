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

#include "exec.h"
#include "lexer.h"


int valery_exec_program(char *path, char *args, int stream[2])
{
    int status;
    int return_code = 0;
    char *args_cpy = args;
    /* env unused in this case */
    char *env[] = {NULL, NULL, NULL};
    int stream_is_pipe = 0;

    if (stream[0] != STDIN_FILENO || stream[1] != STDOUT_FILENO) {
        pipe(stream);
        stream_is_pipe = 1;
    }

    /* args being an empty string results in undefined behavior */
    if (strcmp(args_cpy, "") == 0)
        args_cpy = NULL;

    char *full[] = {path, args_cpy};

    pid_t new_pid = fork();
    if (new_pid == CHILD_PID) {
        /* if read is to pipe, point stdin to the pipe's read end */
        if (stream[0] != STDIN_FILENO)
            dup2(stream[0], STDIN_FILENO);

        /* if write is to pipe, redirect stdout to the pipe's write end */
        if (stream[1] != STDOUT_FILENO)
            dup2(stream[1], STDOUT_FILENO);
        
        if (stream_is_pipe) {
            close(stream[0]);
            close(stream[1]);
        }

        return_code = execve(path, full, env);
        return_code == -1 ? exit(EXIT_FAILURE) : exit(EXIT_SUCCESS);
    }

    if (stream_is_pipe) {
        close(stream[0]);
        close(stream[1]);
    }

    waitpid(new_pid, &status, 0);

    return status != 0;
}

int valery_exec_buffer(struct tokens_t *tokens)
{
    int stream[2];
    stream[0] = STDIN_FILENO;
    stream[1] = STDOUT_FILENO;

    printf("\nTOKENS!!:\n");
    for (size_t i = 0; i < tokens->i; i++) {
        printf("Token num %ld, val: %s, type: %d\n", i, tokens->token_arr[i], tokens->is_op[i]);
    }

    //split_buffer(input_buffer, cmd, args);
    //snprintf(full_cmd, 8192, "%s/%s", env->PATH, cmd);


    //rc = valery_exec(full_cmd, args, stream);
    return 0;
}
