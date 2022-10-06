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
#include <stdbool.h>            // bool type

#include "parser.h"
#include "lex.h"                // struct tokenlist_t type
#include "../vcommon.h"         // errors


/* globals */
struct tokenlist_t *tl_cpy;
size_t tl_pos = 0;      // global for simplicity, may not be ideal going forward


/* types */
struct stmt_t {
    ASTNodeHead head;
};

struct expr_t {
    ASTNodeHead head;
};


/* functions */
static void *ast_line();
static void *ast_primary();
static void *ast_unary();

static struct token_t get_current_token()
{
    return *tl_cpy->tokens[tl_pos];
}

static bool check(enum tokentype_t type)
{
    if (tl_cpy->size >= tl_cpy->capacity)
        return false;

    // should this be abstracted out?
    return tl_cpy->tokens[tl_pos]->type == type;
}

static void consume(enum tokentype_t type, char *err_msg)
{
    if (!check(type))
        valery_exit_parse_error(err_msg);

    tl_pos++;
}

static bool match(enum tokentype_t *types, unsigned int n)
{
    enum tokentype_t type;
    for (unsigned int i = 0; i < n; i++) {
        type = types[i];
        if (check(type)) {
            tl_pos++;
            return true;
        }
    }

    return false;
}

static struct token_t *previous()
{
    return tl_cpy->tokens[tl_pos - 1];
}

static void *ast_program()
{
    void *line = ast_line();
    consume(T_EOF, "error 100");
    return line;
}

static void *ast_line()
{
    return ast_unary();
}

static void *ast_unary()
{
    /*
     * unary           ->      "!" unary
     *                  |       primary ;
     */

    enum tokentype_t m[] = { T_BANG };
    if (match(m, 1)) {
        struct token_t *op = previous();
        Expr *right = ast_unary();

        struct ast_unary_t *expr = malloc(sizeof(struct ast_unary_t));
        expr->head.type = UNARY;
        expr->op = op;
        expr->right = right;
        return expr;
    }

    return ast_primary();
}

static void *ast_primary()
{
    enum tokentype_t m[] = { T_STRING, T_NUMBER };
    if (match(m, 2)) {
        struct token_t *prev = previous();
        struct ast_literal_t *expr = malloc(sizeof(struct ast_literal_t));
        expr->head.type = LITERAL;
        expr->type = prev->type;
        expr->literal = prev->literal;
        expr->literal_size = prev->literal_size;
        return expr;
    }
    return NULL;
}

void *parse(struct tokenlist_t *tl)
{
    tl_cpy = tl;

    /* start recursive descent */
    void *res = ast_program();
    return res;
}
