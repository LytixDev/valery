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
#include "../builtins/builtins.h"


#define DEBUG

int valery_exec_program(char *program_name, char *argv[], int argc, struct env_t *env)
{
#ifdef DEBUG
    printf("'%s' ", program_name);
    for (int i = 0; i < argc; i++)
        printf("'%s' ", argv[i]);

    printf("\n");
#endif

    int status;
    int rc;
    int return_code = 0;
    char *found_path;
    // TODO: make memory robust
    char command_with_path[1024];
    // TODO: add environment variables
    char *environ[] = {NULL, NULL, NULL};

    rc = which(program_name, env->paths, env->current_path, &found_path);
    if (rc != COMMAND_IN_PATH) {
        fprintf(stderr, "valery: command not found '%s'\n", program_name);
        env->exit_code = 1;
        return 1;
    }

    /* command has been found in path and found_path should poit to the address containg the string */
    snprintf(command_with_path, 1024, "%s/%s", found_path, program_name);

    /*
     * full must contain program name and an argument.
     * last argument must be NULL to signify end of pointer arr.
     * ex: full = { "/bin/ls", "-la", "/", NULL }
     */
    char *full[2 + argc];
    full[0] = command_with_path;

    for (int i = 1; i < argc + 1; i++)
        full[i] = argv[i - 1];

    /* last pointer always NULL */
    full[1 + argc] = NULL;

    pid_t new_pid = fork();
    if (new_pid == CHILD_PID) {
        return_code = execve(command_with_path, full, environ);
        env->exit_code = return_code;
        //TODO: is this necessary?
        return_code == -1 ? exit(EXIT_FAILURE) : exit(EXIT_SUCCESS);
    }

    waitpid(new_pid, &status, 0);
    return status != 0;
}

int valery_eval_token(char *program_name, char *argv[], int argc, struct env_t *env, struct hist_t *hist)
{
    int rc;
    //TODO: there has to be a cleaner way?
    /* check if program is shell builtin */
    if (strcmp(program_name, "which") == 0) {
        // TODO: use all args
        rc = which(argv[0], env->paths, env->current_path, NULL);
    } else if (strcmp(program_name, "cd") == 0) {
        // TODO: use all args
        rc = cd(argv[0]);
    } else if (strcmp(program_name, "history") == 0) {
        rc = history(hist);
    } else if (strcmp(program_name, "help") == 0) {
        rc = help();
    } else {
        /* attempt to execute program from path */
        rc = valery_exec_program(program_name, argv, argc, env);
    }
    return rc;
}

int str_to_argv(char *str, char **argv, int *argv_cap)
{
    int argc = 0;
    bool skip = false;
    while (*str != 0) {
        if (*str == '"')
            skip = !skip;

        if (!skip && *str == ' ') {
            *str = 0;
            // TODO: find next non backspace instead of assuming there is always only one backspace
            // example: '  -la' should be evaluated to 'la' and not ' ' and '-la'
            argv[argc++] = ++str;
            if (argc >= *argv_cap) {
                *argv_cap += 8;
                argv = (char **) realloc(argv, *argv_cap * sizeof(char *));
            }
        } else {
            str++;
        }
    }

    for (int i = 0; i < argc; i++) {
        argv[i] = trim_edge(argv[i], '"');
    }


    return argc;
}

int valery_parse_tokens(struct tokenized_str_t *ts, struct env_t *env, struct hist_t *hist)
{
    int rc;
    char **argv = (char **) malloc(8 * sizeof(char *));
    int argv_cap = 8;
    int argc = 0;
    size_t i = 0;
    int stream_in = STDIN_FILENO;
    int stream_out = STDOUT_FILENO;
    token_t *t;

    /* flags. should this be a struct? */
    bool pipe = false;
    
    do { 
        t = ts->tokens[i];

        /* previous token was pipe, therefore read input and input from pipe */
        if (pipe) {

        }

        /* check if next token is pipe */
        if (i + 1 <= ts->size)
            pipe = ts->tokens[i + 1]->type == O_PIPE;



        /* set_flag(type) */
        if (t->type == O_NONE) {
            argc = str_to_argv(t->str_start, argv, &argv_cap);
            rc = valery_eval_token(t->str_start, argv, argc, env, hist);

        } else if (t->type == O_AND) {
            if (env->exit_code != 0)
                break;

        } else if (t->type == O_OR) {
            if (env->exit_code == 0)
                break;
        }

    } while (i++ < ts->size);

    free(argv);

    return 0;

    /* TODO: parse buffer, handle operands and handle different pipes/streams */
    //int no_more_operands;
    //int pos = 0;
    //int rc = 0;

    //// todo: realloc when necessar
    //char *cmd = (char *) malloc(1024 * sizeof(char));
    //char *args = (char *) malloc(1024 * sizeof(char));

    //while (1) {
    //    no_more_operands = 1;
    //    memset(cmd, 0, 1024);
    //    memset(args, 0, 1024);

    //    if (tokens->token_type[pos] != O_NONE) {
    //        fprintf(stderr, "valery: invalid starting token '%s'\n", tokens->token_arr[pos]);
    //        rc = 2;
    //        break;
    //    }

    //    /* set cmd to be first token */
    //    snprintf(cmd, 1024, "%s", tokens->token_arr[pos++]);

    //    /* check if there are no more operands */
    //    for (int i = pos; i < tokens->i; i++) {
    //        if (tokens->token_type[i] != O_NONE) {
    //            no_more_operands = 0;
    //            break;
    //        }
    //    }
    //    
    //    /* copy subsequent tokens until next operand into args */
    //    while (tokens->token_type[pos] == O_NONE && pos < tokens->i) {
    //        // TODO: ensure memory safety and seperation of args by whitespace
    //        strcat(args, tokens->token_arr[pos++]);
    //    }

    //    rc = valery_eval_tokens(cmd, args, env, hist);

    //    /* move past operand */
    //    pos++;
    //    if (pos >= tokens->i)
    //        break;
    //}

    //free(cmd);
    //free(args);
    //return rc;
}
