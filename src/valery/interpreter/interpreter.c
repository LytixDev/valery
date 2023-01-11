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

static void simple_command(struct ListExpr *expr)
{
    //valery_exec_program(darr_get_size(expr->exprs), NULL);
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
    //switch (expr->head.token->type) {
    //    case T_PIPE:
    //        pipe_sequence(expr);
    //        break;
    //    case T_AND_IF:
    //        and_if(expr);
    //        break;
    //    case T_PIPE_PIPE:
    //        and_or(expr);
    //        break;

    //    default:
    //        fprintf(stderr, "binary interpret err");
    //        break;
    //}
}

static int interpret_node(ASTNodeHead *expr)
{
    switch (expr->type) {
        case AST_UNARY:
            interpret_unary((struct UnaryExpr *)expr);
            break;
        case AST_BINARY:
            interpret_binary((struct BinaryExpr *)expr);
            break;

        case AST_LITERAL:
            //simple_command((struct LiteralExpr *)expr);
            break;

        case AST_LIST:
            simple_command((struct ListExpr *)expr);
            break;

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
