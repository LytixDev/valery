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
#include <string.h>

#include "valery/interpreter/ast.h"
#include "valery/interpreter/interpreter.h"
#include "valery/interpreter/lexer.h"
#include "valery/interpreter/parser.h"
#include "valery/interpreter/impl/exec.h"
#include "lib/nicc/nicc.h"
#include "valery/valery.h"

int glob_exit_code = 0;
struct ht_t *global_vars = NULL;

static void execute(struct Stmt *stmt);
static void *evaluate(struct Expr *expr);


static void simple_command(struct CommandExpr *expr)
{
    int argc = (int)darr_get_size(expr->exprs);
    struct darr_t *argv = darr_malloc();
    for (int i = 0; i < argc; i++) {
        struct Expr *e = darr_get(expr->exprs, i);
        void *literal = evaluate(e);
        darr_append(argv, literal);
    }

    glob_exit_code = valery_exec_program(argc, (char **)darr_raw_ret(argv));
}

static void *and_if(struct BinaryExpr *expr)
{
    evaluate(expr->left);
    if (glob_exit_code == 0)
        evaluate(expr->right);

    return NULL;
}

static void *eval_unary(struct UnaryExpr *expr)
{
    return NULL;
}

static void *eval_binary(struct BinaryExpr *expr)
{
    switch (expr->operator_->type) {
        case T_AND_IF:
            return and_if(expr);
            break;

        default:
            valery_exit_internal_error("goooo");
            break;
    }
    
    return NULL;
}

static void interpret_list(struct CommandExpr *expr)
{
    if (expr->head.type == EXPR_COMMAND)
        simple_command(expr);
    else
        valery_exit_internal_error("oop");
}

static void *eval_var(struct VariableExpr *expr)
{
    void *value = ht_sget(global_vars, expr->name->lexeme);
    if (value == NULL)
        return "";

    return value;
}

static bool is_truthy(struct Expr *expr)
{
}

static void exec_var(struct VarStmt *stmt)
{
    void *value = evaluate(stmt->initializer);
    ht_sset(global_vars, stmt->name->lexeme, value);
}

static void exec_if(struct IfStmt *stmt)
{
    void *e = evaluate(stmt->condition);
    // check if e is truthy
    execute(stmt->then_branch);
}

static void *expand(struct darr_t *expansions)
{
    //TODO: add dynamic str type in nicc
    char *final = malloc(1024);
    size_t bound = darr_get_size(expansions);

    for (size_t i = 0; i < bound; i++) {
        struct expansion_t *e = darr_get(expansions, i);
        if (e->type == ET_LITERAL) {
            strcat(final, e->value);
        } else if (e->type == ET_PARAMETER) {
            void *var_value = ht_sget(global_vars, (char *)e->value + 1);
            if (var_value != NULL)
                strcat(final, var_value);
        }
    }
    return final;
}

//lol
static void *eval_literal(struct LiteralExpr *expr)
{
    if (expr->value_type != LIT_EXPANSION)
        return expr->value;

    return expand(expr->value);
    //struct Value *value = malloc(sizeof(struct Value));

    //if (expr->value_type != LIT_EXPANSION) {
    //    value->value = expr->value;
    //    if (expr->value_type == LIT_INT)
    //        value->type = VAL_INT;
    //    else
    //        value->type = VAL_STRING;

    //    return value;
    //}

    //char *expanded_str = expand(expr->value);
    //value->value = expanded_str;
    //value->type = VAL_STRING;
    //return value;
}

static void execute(struct Stmt *stmt)
{
    /* empty */
    //if (stmt == NULL)
    //    return;

    switch (stmt->type) {
        case STMT_EXPRESSION:
            evaluate(((struct ExpressionStmt *)stmt)->expression);
            break;
        case STMT_VAR:
            exec_var((struct VarStmt *)stmt);
            break;

        case STMT_IF:
            exec_if((struct IfStmt *)stmt);
            break;

        default:
            valery_exit_internal_error("tq");
    }
}


static void *evaluate(struct Expr *expr)
{
    switch (expr->type) {
        case EXPR_UNARY:
            return eval_unary((struct UnaryExpr *)expr);
            break;
        case EXPR_BINARY:
            return eval_binary((struct BinaryExpr *)expr);
            break;

        case EXPR_LITERAL:
            return eval_literal(((struct LiteralExpr *)expr));
            break;

        case EXPR_COMMAND:
            /* should this be a stmt? */
            interpret_list((struct CommandExpr *)expr);
            break;

        case EXPR_VARIABLE:
            return eval_var((struct VariableExpr *)expr);
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

    global_vars = ht_alloc(1024);    

    int bound = darr_get_size(statements);
    for (int i = 0; i < bound; i++)
        execute((struct Stmt *)darr_get(statements, i));

    return glob_exit_code;
}
