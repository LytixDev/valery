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
 *  You should have received a copy of the GNU General Public License along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include <stdlib.h>

#include "valery/interpreter/ast.h"
#include "valery/interpreter/lexer.h"
#include "valery/interpreter/parser.h"
#include "valery/interpreter/parser_utils.h"
#include "valery/valery.h"

/* spec */
static void *program(void);
//static void *complete_commands(void);
static void *complete_command(void);
static void *list(void);
static void *and_or(void);
//static void *pipeline(void);
static void *pipe_sequence(void);
static void *command(void);
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
//static void *linebreak(void);
//static void *separator_op(void);
//static void *separator(void);
//static void *sequential_sep(void);

/* globals */
struct tokenlist_t *tokenlist;


/* functions */

/* 
 * : complete_command EOF ;
 */
static void *program(void)
{
    struct ast_node_t *expr = complete_command();
    consume(T_EOF, "eof err");
    return expr;
}

/*
 * : list ;
 */
static void *complete_command(void)
{
    return list();
}

/*
 * : and_or ;
 */
static void *list(void)
{
    return and_or();
}

/*
 * : pipe_sequence
 * | pipe_sequence ('&&' | '||) pipe_sequence ;
 */
static void *and_or(void)
{
    struct ast_node_t *left = pipe_sequence();

    if (!check(T_AND_IF, T_PIPE_PIPE))
        return left;

    if (!match(T_AND_IF, T_PIPE_PIPE))
        valery_exit_internal_error("dafuq");

    struct token_t *token = previous();
    struct ast_node_t *right = pipe_sequence();

    struct ast_binary_t *expr = expr_malloc(BINARY, token);
    expr->left = left;
    expr->right = right;
    return expr;
}

/*
 * : command '|' command
 * | command ;
 */
static void *pipe_sequence(void)
{
    struct ast_node_t *left = command();

    if (!check(T_PIPE))
        return left;

    struct token_t *token = consume(T_PIPE, "pipe err");
    struct ast_node_t *right = command();

    struct ast_binary_t *expr = expr_malloc(BINARY, token);
    expr->left = left;
    expr->right = right;
    return expr;
}

/*
 * : simple_command ;
 */
static void *command(void)
{
    return simple_command();
}

/*
 * : WORD 
 * | WORD simple_command ;
 */
static void *simple_command(void)
{
    struct token_t *token = consume(T_WORD, "word err");
    struct ast_unary_t *expr = expr_malloc(UNARY, token);
    expr->right = NULL;
    if (check(T_WORD))
        expr->right = simple_command();
    return expr;
}

struct ast_node_t *parse(struct tokenlist_t *tl)
{
    tokenlist = tl;
    struct ast_node_t *head = program();
    return head;
}
