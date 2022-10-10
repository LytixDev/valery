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

#include "valery/exec.h"
#include "builtins/builtins.h"


int valery_exec_program(char *program_name, char *argv[], int argc, struct env_t *env, struct exec_ctx *e_ctx)
{
    int status;
    int rc;
    int return_code;
    char *found_path;
    char *command_with_path;
    char tmp[1024];

    char *environ[env->env_vars->capacity];
    env_gen(env->env_vars, environ);

    rc = which_single(program_name, env->paths->paths, env->paths->size, &found_path);
    if (!(rc == COMMAND_IN_PATH || rc == COMMAND_IS_PATH)) {
        fprintf(stderr, "valery: command not found '%s'\n", program_name);
        return 1;
    }

    if (rc == COMMAND_IS_PATH) {
        command_with_path = program_name;
    } else {
        /* command has been found in path and found_path should point to the address containing the string */
        //TODO: make memory save
        snprintf(tmp, 1024, "%s/%s", found_path, program_name);
        command_with_path = tmp;
    }

    /*
     * full must contain program name and an argument.
     * last argument must be NULL to signify end of pointer arr.
     * ex: full = { "/bin/ls", "-la", NULL }
     */
    char *full[2 + argc];
    full[0] = command_with_path;

    for (int i = 0; i < argc; i++)
        full[i + 1] = argv[i];

    /* last pointer always NULL */
    full[1 + argc] = NULL;

    #ifdef DEBUG
    for (int i = 0; i < argc + 1; i++)
        print_debug("argv['%d'] = '%s'\n", i, full[i]);
    #endif /* DEBUG */

    pid_t new_pid = fork();
    if (new_pid == CHILD_PID) {
        /* dup read end */
        if (e_ctx->read_stream != ST_NONE) {
            dup2(e_ctx->streams[e_ctx->read_stream][READ_END], STDIN_FILENO);
        }
        /* dup write end */
        if (e_ctx->write_stream != ST_NONE) {
            dup2(e_ctx->streams[e_ctx->write_stream][WRITE_END], STDOUT_FILENO);
        }

        /* close all open file descriptors */
        if (!(e_ctx->flags & SF_ADAM_VACANT)) {
            close(e_ctx->streams[ST_ADAM][READ_END]);
            close(e_ctx->streams[ST_ADAM][WRITE_END]);
        }
        if (!(e_ctx->flags & SF_SETH_VACANT)) {
            close(e_ctx->streams[ST_SETH][READ_END]);
            close(e_ctx->streams[ST_SETH][WRITE_END]);
        }

        return_code = execve(command_with_path, full, environ);
    }

    terminate_pipe(e_ctx);

    waitpid(new_pid, &status, 0);
    return status != 0;
}

bool valery_eval_token(char *program_name, char *argv[], int argc, struct env_t *env, struct hist_t *hist)
{
    int rc;
    /* check if program is shell builtin */
    if (strcmp(program_name, "which") == 0)
        rc = which(argv, argc, env->paths->paths, env->paths->size);
    else if (strcmp(program_name, "cd") == 0)
        rc = cd(argv[0]);
    else if (strcmp(program_name, "history") == 0)
        rc = history(hist, argc > 0);
    else if (strcmp(program_name, "pwd") == 0)
        rc = pwd(NULL);
    else if (strcmp(program_name, "help") == 0)
        rc = help();
    else
        return false;

    return true;
}

//int valery_parse_tokens(struct source_t *ts, struct env_t *env, struct hist_t *hist)
//{
//    token_t *t;
//    operands_t next_type;
//    int argv_cap = 8;
//    int argc;
//    char **argv = (char **) vmalloc(8 * sizeof(char *));
//    /* initialize exec_ctx to have vacant streams */
//    exec_ctx e_ctx = { .flags = SF_ADAM_VACANT | SF_SETH_VACANT, .read_stream = ST_NONE, .write_stream = ST_NONE };
//
//    for (size_t i = 0; i < ts->size; i++) {
//        t = ts->tokens[i];
//
//        /* only look ahead in if it is not the last token */
//        if (i != ts->size - 1)
//            next_type = ts->tokens[i + 1]->type;
//        else
//            next_type = O_NONE;
//
//        update_exec_flags(&e_ctx, t->type, next_type);
//
//        if (t->type == O_NONE) {
//            argc = str_to_argv(t->str_start, argv, &argv_cap);
//            if (!valery_eval_token(t->str_start, argv, argc, env, hist))
//                valery_exec_program(t->str_start, argv, argc, env, &e_ctx);
//        }
//    }
//
//    free(argv);
//    return 0;
//}

void new_pipe(struct exec_ctx *e_ctx)
{
    int rc;
    enum stream_t st;
    /* pipe first non-vacant stream */
    if (e_ctx->flags & SF_ADAM_VACANT) {
        st = ST_ADAM;
        e_ctx->flags ^= SF_ADAM_VACANT;
    } else if (e_ctx->flags & SF_SETH_VACANT) {
        st = ST_SETH;
        e_ctx->flags ^= SF_SETH_VACANT;
    } else {
        fprintf(stderr, "valery internal error: both streams occupied\n");
        exit(EXIT_FAILURE);
    }

    rc = pipe(e_ctx->streams[st]);
    if (rc == -1) {
        fprintf(stderr, "valery internal error: call to POSIX C library function pipe() failed\n");
        exit(EXIT_FAILURE);
    }

    /* creating a new pipe will always override the current write end */
    e_ctx->write_stream = st;
}

void terminate_pipe(struct exec_ctx *e_ctx)
{
    enum stream_t st = ST_NONE;
    if (e_ctx->flags & SF_ADAM_CLOSE) {
        e_ctx->flags ^= SF_ADAM_CLOSE;
        st = ST_ADAM;
    } else if (e_ctx->flags & SF_SETH_CLOSE) {
        e_ctx->flags ^= SF_SETH_CLOSE;
        st = ST_SETH;
    }

    if (st != ST_NONE) {
        close(e_ctx->streams[st][READ_END]);
        close(e_ctx->streams[st][WRITE_END]);
        st == ST_ADAM ? (e_ctx->flags |= SF_ADAM_VACANT) : (e_ctx->flags |= SF_SETH_VACANT);
    }
}

//void update_exec_flags(struct exec_ctx *e_ctx, operands_t type, operands_t next_type)
//{
//    if (type == O_PIPE) {
//        /* write stream was set in previous call */
//        e_ctx->read_stream = e_ctx->write_stream;
//        e_ctx->write_stream = ST_NONE;
//
//        if (e_ctx->read_stream == ST_ADAM)
//            e_ctx->flags |= SF_ADAM_CLOSE;
//        else
//            e_ctx->flags |= SF_SETH_CLOSE;
//    }
//
//    /* check if next token wants to redirect output */
//    // TODO: currently only look ahead for pipe. add: O_OUTP '>',O_OUPP '>>', O_INP '<' and O_INPP '<<'
//    if (next_type == O_PIPE) {
//        new_pipe(e_ctx);
//    }
//}

