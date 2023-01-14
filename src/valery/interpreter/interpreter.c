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
#include "valery/valery.h"

int glob_exit_code = 0;

static int interpret_node(struct Expr *expr);
static void *interpret_node_to_literal(struct Expr *expr);

static void simple_command(struct CommandExpr *expr)
{
    int argc = (int)darr_get_size(expr->exprs);
    struct darr_t *argv = darr_malloc();
    /*
     * NOTE:
     * essentially we are resolving all the nodes in the list that will become argv
     * right now later down the chain they are all assumed to be literals, but later on an argv can
     * f.ex be a subshell or some other statement that needs to be resolved and return a literal
     */
    for (int i = 0; i < argc; i++) {
        struct Expr *e = darr_get(expr->exprs, i);
        void *res = interpret_node_to_literal(e);
        darr_append(argv, res);
    }

    glob_exit_code = valery_exec_program(argc, (char **)darr_raw_ret(argv));
}

static void pipe_sequence(struct BinaryExpr *expr)
{
    /* 
     * open pipe 
     * exec left of pipe and redirect output to write end of pipe
     * close write end of pipe
     * exec right and redirect input to read end of pipe
     * close read end of pipe
     */
}

static void and_if(struct BinaryExpr *expr)
{
    interpret_node(expr->left);
    if (glob_exit_code == 0)
        interpret_node(expr->right);
}

static void and_or(struct BinaryExpr *expr)
{
}

/* main interpreter functions */
static void interpret_unary(struct UnaryExpr *expr)
{
    /* FOR NOW: assume all unaries are simple commands */
    //simple_command(expr);
}

static void interpret_binary(struct BinaryExpr *expr)
{
    switch (expr->operator_->type) {
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
            valery_exit_internal_error("goooo");
            break;
    }
}

static void interpret_list(struct CommandExpr *expr)
{
    if (expr->head.type == EXPR_COMMAND)
        simple_command(expr);
    else
        valery_exit_internal_error("oop");

}

static void *interpret_node_to_literal(struct Expr *expr_head)
{
    /* NOTE: assuming all here are literals, BAD */
    if (expr_head->type != EXPR_LITERAL)
        valery_exit_internal_error("node should be lit!!!");

    return ((struct LiteralExpr *)expr_head)->value;
}

static int interpret_node(struct Expr *expr)
{
    switch (expr->type) {
        case EXPR_UNARY:
            interpret_unary((struct UnaryExpr *)expr);
            break;
        case EXPR_BINARY:
            interpret_binary((struct BinaryExpr *)expr);
            break;

        case EXPR_LITERAL:
            //simple_command((struct LiteralExpr *)expr);
            break;

        case EXPR_COMMAND:
            interpret_list((struct CommandExpr *)expr);
            break;

        case EXPR_ENUM_COUNT:
            // ignore
            break;
    }

    return 0;
}

int interpret(struct Expr *expr_head)
{
#ifdef DEBUG
    printf("\n--- interpreter start ---\n");
#endif
    interpret_node(expr_head);
    return 0;
}
