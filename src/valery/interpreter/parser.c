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

#include <stdlib.h>

#include "lib/nicc/nicc.h"
#include "valery/interpreter/ast.h"
#include "valery/interpreter/lexer.h"
#include "valery/interpreter/parser.h"
#include "valery/interpreter/parser_utils.h"
#include "valery/valery.h"


/* spec */
//static void *program(void);
//static void *complete_commands(void);
//static void *complete_command(void);
//static void *list(void);
//static void *and_or(void);
//static void *and_if(void);
//static void *pipeline(void);
//static void *pipe_sequence(void);
//static void *command(void);
//static void *compound_command(void);
//static void *subshell(void);
//static void *compound_list(void);
//static void *term(void);
//static void *for_clause(void);
//static void *name(void);
//static void *in(void);
//static void *wordlist(void);
//static void *case_clause(void);
//static void *case_list_ns(void);
//static void *case_list(void);
//static void *case_item_ns(void);
//static void *case_item(void);
//static void *pattern(void);
//static void *if_clause(void);
//static void *else_part(void);
//static void *while_clause(void);
//static void *until_clause(void);
//static void *function_definition(void);
//static void *function_body(void);
//static void *fname(void);
//static void *brace_group(void);
//static void *do_group(void);
static void *simple_command(void);
//static void *cmd_name(void);
//static void *cmd_word(void);
//static void *cmd_prefix(void);
//static void *cmd_suffix(void);
//static void *redirect_list(void);
//static void *io_redirect(void);
//static void *io_file(void);
//static void *filename(void);
//static void *io_here(void);
//static void *here_end(void);
//static void *newline_list(void);
static void *linebreak(void);
//static void *separator_op(void);
//static void *separator(void);
//static void *sequential_sep(void);

/* globals */
struct tokenlist_t *tokenlist;

static struct Stmt *program(void);
static struct Expr *and_if(void);
static struct Expr *command(void);

static struct Stmt *program(void)
{
    struct ExpressionStmt *stmt = (struct ExpressionStmt *)stmt_alloc(STMT_EXPRESSION, NULL);
    struct Expr *expr = and_if();
    stmt->expression = expr;
    consume(T_NEWLINE, "newline expected");
    return (struct Stmt *)stmt;
}

static struct Expr *and_if(void)
{
    void *condition = command();
    if (match(T_AND_IF)) {
        struct BinaryExpr *expr = (struct BinaryExpr *)expr_alloc(EXPR_BINARY, NULL);
        struct token_t *prev = previous();
        void *then = command();
        expr->left = condition;
        expr->operator_ = prev;
        expr->right = then;
        return (struct Expr *)expr;
    }

    return condition;
}

static struct Expr *command(void)
{
    struct CommandExpr *expr = (struct CommandExpr *)expr_alloc(EXPR_COMMAND, NULL);
    while (match(T_WORD, T_STRING)) {
        struct token_t *prev = previous();
        struct LiteralExpr *expr_lit = (struct LiteralExpr *)expr_alloc(EXPR_LITERAL, prev);
        darr_append(expr->exprs, expr_lit);
    }
    return (struct Expr *)expr;
}

struct darr_t *parse(struct tokenlist_t *tl)
{
    tokenlist = tl;
    struct darr_t *statements = darr_malloc();
    while (!check(T_EOF))
        darr_append(statements, program());
    return statements;
}
