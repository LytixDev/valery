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


//#define DEBUG

int valery_exec_program(char *program_name, char *argv[], int argc, struct env_t *env, struct exec_ctx *e_ctx)
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
        /* dup read end */
        if (e_ctx->read_stream != S_NOT_IN_USE) {
            dup2(e_ctx->streams[e_ctx->read_stream][READ_END], STDIN_FILENO);
        }
        /* dup write end */
        if (e_ctx->write_stream != S_NOT_IN_USE) {
            dup2(e_ctx->streams[e_ctx->write_stream][WRITE_END], STDOUT_FILENO);
        }

        /* close all open file descriptors */
        if (!(e_ctx->flags & ADAM_VACANT)) {
            close(e_ctx->streams[S_ADAM][READ_END]);
            close(e_ctx->streams[S_ADAM][WRITE_END]);
        }
        if (!(e_ctx->flags & SETH_VACANT)) {
            close(e_ctx->streams[S_SETH][READ_END]);
            close(e_ctx->streams[S_SETH][WRITE_END]);
        }

        return_code = execve(command_with_path, full, environ);
        env->exit_code = return_code;
    }

    /* terminate file descriptors */
    if (e_ctx->flags & ADAM_CLOSE) {
        e_ctx->flags ^= ADAM_CLOSE;
        terminate_pipe(e_ctx, S_ADAM);
    }

    if (e_ctx->flags & SETH_CLOSE) {
        e_ctx->flags ^= SETH_CLOSE;
        terminate_pipe(e_ctx, S_SETH);
    }
    //if (e_ctx->flags & STREAM1_CLOSE) {
    //    close(e_ctx->stream1[READ_END]);
    //    close(e_ctx->stream1[WRITE_END]);
    //    e_ctx->flags ^= STREAM1_CLOSE;
    //    /* */
    //}
    waitpid(new_pid, &status, 0);
    return status != 0;
}

int valery_eval_token(char *program_name, char *argv[], int argc, struct env_t *env, struct hist_t *hist, struct exec_ctx *e_ctx)
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
        rc = valery_exec_program(program_name, argv, argc, env, e_ctx);
    }
    return rc;
}

int valery_parse_tokens(struct tokenized_str_t *ts, struct env_t *env, struct hist_t *hist)
{
    int rc;
    token_t *t;
    operands_t next_type;
    int argv_cap = 8;
    int argc = 0;
    char **argv = (char **) malloc(8 * sizeof(char *));
    /* initialize exec_ctx to have vacant streams */
    exec_ctx e_ctx = { .flags = ADAM_VACANT | SETH_VACANT, .read_stream = S_NOT_IN_USE, .write_stream = S_NOT_IN_USE };

    for (size_t i = 0; i <= ts->size; i++) {
        t = ts->tokens[i];

        /* only look ahead in if it is not the last token */
        if (i != ts->size)
            next_type = ts->tokens[i + 1]->type;
        else
            next_type = O_NONE;

        update_exec_flags(&e_ctx, t->type, next_type);

        if (t->type == O_NONE) {
            argc = str_to_argv(t->str_start, argv, &argv_cap);
            valery_eval_token(t->str_start, argv, argc, env, hist, &e_ctx);
        }
    }


    free(argv);

    return 0;
    /*
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
    */
}

void new_pipe(struct exec_ctx *e_ctx)
{
    stream_t st;
    /* pipe first non vacant stream */
    if (e_ctx->flags & ADAM_VACANT) {
        st = S_ADAM;
        e_ctx->flags ^= ADAM_VACANT;
    } else if (e_ctx->flags & SETH_VACANT) {
        st = S_SETH;
        e_ctx->flags ^= SETH_VACANT;
    } else {
        fprintf(stderr, "BOTH STREAMS OCCUPIED, ERROR\n");
        exit(EXIT_FAILURE);
    }

    pipe(e_ctx->streams[st]);
    /* creating a new pipe will always override the current write end */
    e_ctx->write_stream = st;
}

void terminate_pipe(struct exec_ctx *e_ctx, stream_t st)
{
    //TODO: can call to close() fail?
    close(e_ctx->streams[st][READ_END]);
    close(e_ctx->streams[st][WRITE_END]);
    if (st == S_ADAM) {
        //e_ctx->flags ^= ADAM_CLOSE;
        e_ctx->flags |= ADAM_VACANT;
    } else {
        //e_ctx->flags ^= SETH_CLOSE;
        e_ctx->flags |= SETH_VACANT;
    }
}

void update_exec_flags(struct exec_ctx *e_ctx, operands_t type, operands_t next_type)
{
    // TODO: currently assumes inputted tokens are valid.
    if (type == O_PIPE) {
        e_ctx->flags ^= NEXT_IS_PIPE;
        e_ctx->flags |= CAME_FROM_PIPE;
        /* write stream was set in previous call */
        e_ctx->read_stream = e_ctx->write_stream;
        e_ctx->write_stream = S_NOT_IN_USE;

        if (e_ctx->read_stream == S_ADAM)
            e_ctx->flags |= ADAM_CLOSE;
        else
            e_ctx->flags |= SETH_CLOSE;
    }

    /* check if next token wants to redirect output */
    // TODO: currently only look ahead for pipe. add: O_OUTP '>',O_OUPP '>>', O_INP '<' and O_INPP '<<'
    if (next_type == O_PIPE) {
        e_ctx->flags |= NEXT_IS_PIPE;
        new_pipe(e_ctx);
    }
    
    if (e_ctx->flags & CAME_FROM_PIPE) {
        //if (!(e_ctx->flags & STREAM1_VACANT)) {
        //    e_ctx->flags |= STREAM1_CLOSE;
        //}
    }
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

