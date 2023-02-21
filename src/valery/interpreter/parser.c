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
struct darr_t *tokens;

static struct Stmt *line(void);
static struct Stmt *stmt_func(void);
static struct Stmt *if_stmt(void);
static struct Stmt *variable_declaration(void);
static struct Expr *and_if(void);
static struct Expr *expr_func(void);
static struct Expr *command(void);
static struct Expr *primary(void);

static struct Stmt *line(void)
{
    consume(T_NEWLINE, "newline expected");
    /* match the next newlines if present */
    while (match(T_NEWLINE));

    struct Stmt *stmt = stmt_func();
    return stmt;
}

static struct Stmt *stmt_func(void)
{
    if (check(T_IF))
        return if_stmt();
    return variable_declaration();
}

static struct Stmt *if_stmt(void)
{
    consume(T_IF, "expected if token");
    struct Expr *condition = expr_func();
    consume(T_NEWLINE, "expected newline token");
    consume(T_THEN, "expected then token");
    consume(T_NEWLINE, "expected newline token");
    struct Stmt *then_branch = stmt_func(); 
    consume(T_NEWLINE, "expected newline token");
    consume(T_FI, "expected fi token");

    struct IfStmt *stmt = (struct IfStmt *)stmt_alloc(STMT_IF, NULL);
    stmt->condition = condition;
    stmt->then_branch = then_branch;
    stmt->else_branch = NULL;
    return (struct Stmt *)stmt;
}

static struct Stmt *variable_declaration(void)
{
    /* T_WORD T_EQUAL string (should be expr) */
    if (!(check(T_WORD) && check_ahead(1, T_EQUAL) && (check_ahead(2, T_EXPANSION) ||
        check_ahead(2, T_WORD)))) {

        struct ExpressionStmt *e_stmt = (struct ExpressionStmt *)stmt_alloc(STMT_EXPRESSION, NULL);
        e_stmt->expression = and_if();
        return (struct Stmt *)e_stmt;
    }

    struct token_t *name = consume(T_WORD, "expected word");
    consume(T_EQUAL, "expected equal");
    struct Expr *lit;
    if (check(T_EXPANSION))
        lit = expr_alloc(EXPR_LITERAL, consume(T_EXPANSION, "expected expansion"));
    else
        lit = expr_alloc(EXPR_LITERAL, consume(T_WORD, "expected word"));

    struct VarStmt *stmt = (struct VarStmt *)stmt_alloc(STMT_VAR, NULL);
    stmt->name = name;
    stmt->initializer = lit;
    return (struct Stmt *)stmt;
}

static struct Expr *and_if(void)
{
    void *condition = expr_func();
    if (match(T_AND_IF)) {
        struct BinaryExpr *expr = (struct BinaryExpr *)expr_alloc(EXPR_BINARY, NULL);
        struct token_t *prev = previous();
        void *then = expr_func();
        expr->left = condition;
        expr->operator_ = prev;
        expr->right = then;
        return (struct Expr *)expr;
    }

    return condition;
}

static struct Expr *expr_func(void)
{
    if (check(T_TRUE, T_FALSE))
        return primary();
    return command();
}

static struct Expr *command(void)
{
    struct CommandExpr *expr = (struct CommandExpr *)expr_alloc(EXPR_COMMAND, NULL);
    while (match(T_WORD, T_EXPANSION)) {
        struct token_t *prev = previous();
        struct LiteralExpr *expr_lit = (struct LiteralExpr *)expr_alloc(EXPR_LITERAL, prev);
        darr_append(expr->exprs, expr_lit);
    }
    return (struct Expr *)expr;
}

static struct Expr *primary(void)
{
    if (match(T_TRUE)) {
    } else {
        consume(T_FALSE, "expected either true or false");
    }

    struct token_t *prev = previous();
    struct LiteralExpr *expr = (struct LiteralExpr *)expr_alloc(EXPR_LITERAL, prev);
    return (struct Expr *)expr;
}

struct darr_t *parse(struct darr_t *t)
{
    tokens = t;
    struct darr_t *statements = darr_malloc();
    while (!check(T_EOF))
        darr_append(statements, line());
    return statements;
}
