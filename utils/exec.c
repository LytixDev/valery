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

#include "lexer.h"
#include "exec.h"
#include "histfile.h"
#include "../valery.h"
#include "../builtin/builtins.h"


int valery_exec_program(char *program_name, char *args, struct env_t *env)
{
    int status;
    int rc;
    int return_code = 0;
    char *found_path;
    // TODO: make memory robust
    char command_with_path[1024];
    char *args_cpy = args;
    // TODO: add environment variables
    char *environ[] = {NULL, NULL, NULL};

    rc = which(program_name, env->paths, env->total_paths, &found_path);
    if (rc != COMMAND_IN_PATH)
        return 1;

    /* command has been found in path and found_path should poit to the address containg the string */
    snprintf(command_with_path, 1024, "%s/%s", found_path, program_name);

    /* args being an empty string results in undefined behavior */
    if (strcmp(args_cpy, "") == 0)
        args_cpy = NULL;

    char *full[] = {command_with_path, args_cpy};

    pid_t new_pid = fork();
    if (new_pid == CHILD_PID) {
        return_code = execve(command_with_path, full, environ);
        return_code == -1 ? exit(EXIT_FAILURE) : exit(EXIT_SUCCESS);
    }

    waitpid(new_pid, &status, 0);
    return status != 0;
}

int valery_eval_tokens(char *program_name, char *args, struct env_t *env, struct hist_t *hist)
{
    int rc;
    //TODO: there has to be a cleaner way?
    /* check if program is shell builtin */
    if (strcmp(program_name, "which") == 0) {
        rc = which(args, env->paths, env->total_paths, NULL);
    } else if (strcmp(program_name, "cd") == 0) {
        rc = cd(args);
    } else if (strcmp(program_name, "history") == 0) {
        rc = history(hist);
    } else if (strcmp(program_name, "help") == 0) {
        rc = help();
    } else {
        /* attempt to execute program from path */
        rc = valery_exec_program(program_name, args, env);
    }
    return rc;
}

int valery_parse_tokens(struct tokens_t *tokens, struct env_t *env, struct hist_t *hist)
{
    // debug
    //for (int i = 0; i < tokens->i; i++) {
    //    printf("val: '%s', type: %d\n", tokens->token_arr[i], tokens->token_type[i]);
    //}

    /* TODO: parse buffer, handle operands and handle different pipes/streams */
    int no_more_operands;
    int pos = 0;
    int rc = 0;

    // todo: realloc when necessar
    char *cmd = (char *) malloc(1024 * sizeof(char));
    char *args = (char *) malloc(1024 * sizeof(char));

    while (1) {
        no_more_operands = 1;
        memset(cmd, 0, 1024);
        memset(args, 0, 1024);

        if (tokens->token_type[pos] != O_NONE) {
            fprintf(stderr, "valery: invalid starting token '%s'\n", tokens->token_arr[pos]);
            rc = 2;
            break;
        }

        /* set cmd to be first token */
        snprintf(cmd, 1024, "%s", tokens->token_arr[pos++]);

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

        rc = valery_eval_tokens(cmd, args, env, hist);

        /* move past operand */
        pos++;
        if (pos >= tokens->i)
            break;
    }

    free(cmd);
    free(args);
    return rc;
}
