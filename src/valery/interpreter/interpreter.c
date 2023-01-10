/*
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
 *  You should have received a copy of the GNU General Public License along with this program.
 *  If not, see <https://www.gnu.org/licenses/>.
 */
#include <stdio.h>

#include "valery/interpreter/ast.h"
#include "valery/interpreter/lexer.h"
#include "valery/interpreter/parser.h"
#include "valery/interpreter/impl/exec.h"
#include "lib/nicc/nicc.h"

int glob_exit_code = 0;

static int interpret_node(ASTNodeHead *expr);

static void simple_command(struct ast_prog_t *expr)
{
    /* exec command WORD and recursively find arguments by looking right untl NULL */
    //int argc = 0;
    //char *argv[32];

    //struct ast_unary_t *n = expr;
    //while (n != NULL) {
    //    argv[argc++] = n->head.token->lexeme;
    //    n = (struct ast_unary_t *)n->right;     //NOTE: wrong assumption here
    //}

    //argv[argc] = NULL;

    size_t argc = darr_get_size(expr->argv);
    for (size_t i = 0; i < argc; i++)
        printf("%s\n", darr_get(expr->argv, i));
    //void *argv[argc + 1];
    //darr_raw(expr->list, argv);
    //glob_exit_code = valery_exec_program(argc, argv);
}

static void pipe_sequence(struct ast_binary_t *expr)
{
    /* 
     * open pipe 
     * exec left of pipe and redirect output to write end of pipe
     * close write end of pipe
     * exec right and redirect input to read end of pipe
     * close read end of pipe
     */
}

static void and_if(struct ast_binary_t *expr)
{
    interpret_node(expr->left);
    if (glob_exit_code == 0)
        interpret_node(expr->right);
}

static void and_or(struct ast_binary_t *expr)
{
}

/* main interpreter functions */
static void interpret_unary(struct ast_unary_t *expr)
{
    /* FOR NOW: assume all unaries are simple commands */
    //simple_command(expr);
}

static void interpret_binary(struct ast_binary_t *expr)
{
    switch (expr->head.token->type) {
        case T_PIPE:
            pipe_sequence(expr);
            break;
        case T_AND_IF:
            and_if(expr);
            break;
        case T_PIPE_PIPE:
            and_or(expr);
            break;

        default:
            fprintf(stderr, "binary interpret err");
            break;
    }
}

static int interpret_node(ASTNodeHead *expr)
{
    switch (expr->type) {
        case UNARY:
            interpret_unary((struct ast_unary_t *)expr);
            break;
        case BINARY:
            interpret_binary((struct ast_binary_t *)expr);
            break;

        case PROG:
            simple_command((struct ast_prog_t *)expr);

        case ENUM_COUNT:
            // ignore
            break;
    }

    return 0;
}

int interpret(ASTNodeHead *expr)
{
#ifdef DEBUG
    printf("\n--- interpreter start ---\n");
#endif
    interpret_node(expr);
    return 0;
}
