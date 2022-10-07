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
#include "../valery.h"          // errors


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
static void *ast_assignment();
static void *ast_unary();
static void *ast_pipe();
static void *ast_program_sequence();

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

static void *expr_malloc(enum ast_type_t type)
{
    Expr *expr;
    switch (type) {
        case ASSIGNMENT:
            expr = malloc(sizeof(struct ast_assignment_t));
            break;

        case UNARY:
            expr = malloc(sizeof(struct ast_unary_t));
            break;

        case BINARY:
            expr = malloc(sizeof(struct ast_binary_t));
            break;

        case LITERAL:
            expr = malloc(sizeof(struct ast_literal_t));
            break;

        case PROGRAM_SEQUENCE:
            expr = malloc(sizeof(struct ast_program_sequence_t));

        case ENUM_COUNT:
            break;
    }

    expr->type = type;
    return expr;
}

static void *ast_program()
{
    //void *line = ast_line();
    void *line = ast_pipe();
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

        struct ast_unary_t *expr = expr_malloc(UNARY);
        expr->op = op;
        expr->right = right;
        return expr;
    }

    return ast_assignment();
}

static void *ast_assignment()
{
    enum tokentype_t m[] = { T_IDENTIFIER };
    if (match(m, 1)) {
        struct token_t *identifier = previous();
        m[0] = T_EQUAL;
        if (match(m, 1)) {
            Expr *value = ast_assignment();
            struct ast_assignment_t *expr = expr_malloc(ASSIGNMENT);
            expr->name = identifier;
            expr->value = value;
            return expr;
        } else {
            // broken
            tl_pos--;
        }
    }

    return ast_primary();
}

static void *ast_primary()
{
    enum tokentype_t m[] = { T_STRING, T_NUMBER };
    if (match(m, 2)) {
        struct token_t *prev = previous();
        struct ast_literal_t *expr = expr_malloc(LITERAL);
        expr->type = prev->type;
        expr->literal = prev->literal;
        expr->literal_size = prev->literal_size;
        return expr;
    }
    return NULL;
}

static void *ast_pipe()
{
    Expr *left = ast_program_sequence();
    enum tokentype_t m[] = { T_PIPE };
    if (match(m, 1)) {
        struct token_t *op = previous();
        Expr *right = ast_program_sequence();
        struct ast_binary_t *expr = expr_malloc(BINARY);
        expr->left = left;
        expr->op = op;
        expr->right = right;
        return expr;
    }

    return left;
}

static void *ast_program_sequence()
{
    enum tokentype_t m[3] = { T_IDENTIFIER, T_NUMBER, T_STRING };
    if (match(m, 1)) {
        struct token_t *program_name = previous();
        unsigned int argc = 0;
        Expr **argv = malloc(sizeof(Expr *) * 32);
        Expr *arg;
        while (match(m, 3)) {
            tl_pos--;
            arg = ast_primary();
            argv[argc++] = arg;
            if (argc == 32) {
                valery_exit_parse_error("too many args");
            }
        }

        struct ast_program_sequence_t *expr = expr_malloc(PROGRAM_SEQUENCE);
        expr->program_name = program_name;
        expr->argv = argv;
        expr->argc = argc;
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
