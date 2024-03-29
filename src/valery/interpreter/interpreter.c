/*
 *  Copyright (C) 2022-2023 Nicolai Brand
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

static void execute(struct Stmt *stmt);
static void *evaluate(struct Expr *expr);


static void simple_command(struct CommandExpr *expr)
{
    int argc = (int)darr_get_size(expr->exprs);
    struct darr_t *argv = darr_malloc();
    for (int i = 0; i < argc; i++) {
        struct Expr *e = darr_get(expr->exprs, i);
        void *res = evaluate(e);
        darr_append(argv, res);
    }

    glob_exit_code = valery_exec_program(argc, (char **)darr_raw_ret(argv));
}

static void and_if(struct BinaryExpr *expr)
{
    evaluate(expr->left);
    if (glob_exit_code == 0)
        evaluate(expr->right);
}

static void interpret_unary(struct UnaryExpr *expr)
{
}

static void interpret_binary(struct BinaryExpr *expr)
{
    switch (expr->operator_->type) {
        case T_AND_IF:
            and_if(expr);
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

static void execute(struct Stmt *stmt)
{
    switch (stmt->type) {
        case STMT_EXPRESSION:
            evaluate(((struct ExpressionStmt *)stmt)->expression);
            break;
    }
}

static void *evaluate(struct Expr *expr)
{
    switch (expr->type) {
        case EXPR_UNARY:
            interpret_unary((struct UnaryExpr *)expr);
            break;
        case EXPR_BINARY:
            interpret_binary((struct BinaryExpr *)expr);
            break;

        case EXPR_LITERAL:
            return ((struct LiteralExpr *)expr)->value;
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

int interpret(struct darr_t *statements)
{
#ifdef DEBUG
    printf("\n--- interpreter start ---\n");
#endif
    int bound = darr_get_size(statements);
    for (int i = 0; i < bound; i++)
        execute((struct Stmt *)darr_get(statements, i));

    return 0;
}
