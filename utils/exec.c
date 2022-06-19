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
#include "../valery.h"
#include "lexer.h"


int valery_exec_program(char *path, char *args)
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

int valery_exec_buffer(struct tokens_t *tokens, struct ENV *env)
{
    /* TODO: parse buffer, handle operands and handle different pipes/streams */
    int stream[2];
    stream[0] = STDIN_FILENO;
    stream[1] = STDOUT_FILENO;
    int no_more_operands;
    int first = 1;
    int pos = 0;
    int rc = 0;
    // todo: realloc when necessar
    char *cmd = (char *) malloc(1024 * sizeof(char));
    char *args = (char *) malloc(1024 * sizeof(char));

    while (1) {
        no_more_operands = 1;
        memset(args, 0, 1024);

        /*
        printf("\nTOKENS!!:\n");
        for (size_t i = 0; i < tokens->i; i++) {
            printf("Token num %ld, val: %s, type: %d\n", i, tokens->token_arr[i], tokens->token_type[i]);
        }
        */

        if (tokens->token_type[pos] != O_NONE) {
            fprintf(stderr, "valery: invalid starting token '%s'\n", tokens->token_arr[pos]);
            rc = 1;
            break;
        }
        /* set cmd to be first token */
        snprintf(cmd, 1024, "%s/%s", env->PATH, tokens->token_arr[pos++]);

        /* check if there are no more operands */
        for (int i = pos; i < tokens->i; i++) {
            if (tokens->token_type[i] != O_NONE) {
                no_more_operands = 0;
                break;
            }
        }
        
        /* copy subsequent tokens until next operand into args */
        while (tokens->token_type[pos] == O_NONE && pos < tokens->i) {
            // TODO: ensure memory safety and seperation of args by whitespace
            strcat(args, tokens->token_arr[pos++]);
        }

        rc = valery_exec_program(cmd, args);
        if (rc == 1) {
            fprintf(stderr, "valery: command not found '%s'\n", cmd);
            break;
        }

        /* move past operand */
        pos++;
        if (pos >= tokens->i)
            break;
        first = 0;
    }


    free(cmd);
    free(args);
    return rc;
}
