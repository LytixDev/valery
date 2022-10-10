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
#include <stdbool.h>                    // bool type
#include <stdarg.h>                     // va_start, va_arg, va_end 

#include "valery/interpreter/parser.h"
#include "valery/interpreter/lex.h"     // struct tokenlist_t type
#include "valery.h"                     // errors


/* globals */
struct tokenlist_t *tokenlist; // global for simplicity, may not be ideal going forward


/* functions */
static void *ast_line(void);
static void *ast_primary(void);
static void *ast_assignment(void);
static void *ast_unary(void);
static void *ast_pipe(void);
static void *ast_program_sequence(void);


static inline struct token_t get_current_token(void)
{
    return *tokenlist->tokens[tokenlist->pos];
}

static inline bool check(enum tokentype_t type)
{
    if (tokenlist->size >= tokenlist->capacity)
        return false;

    return tokenlist->tokens[tokenlist->pos]->type == type;
}

/*
 * @returns true if next token is any of the given arguments, else false.
 */
static bool check_either(unsigned int n, ...)
{
    enum tokentype_t type;
    va_list args;
    va_start(args, n);

    for (unsigned int i = 0; i < n; i++) {
        type = va_arg(args, enum tokentype_t);
        if (check(type))
            return true;
    }

    va_end(args);
    return false;
}

/*
 * consumes the next token if its type is one of the given arguments.
 * @returns true if successfully consumed a token, else false.
 */
static bool match(unsigned int n, ...)
{
    enum tokentype_t type;
    va_list args;
    va_start(args, n);

    for (unsigned int i = 0; i < n; i++) {
        type = va_arg(args, enum tokentype_t);
        if (check(type)) {
            tokenlist->pos++;
            return true;
        }
    }

    va_end(args);
    return false;
}

static void consume(enum tokentype_t type, char *err_msg)
{
    if (!check(type))
        valery_exit_parse_error(err_msg);

    tokenlist->pos++;
}

static struct token_t *previous()
{
    return tokenlist->tokens[tokenlist->pos - 1];
}

/*
 * allocates space for the given expression type.
 * sets the newly allocated expression's type to the given type.
 */
static void *expr_malloc(enum ast_type_t type)
{
    struct ast_node_t *expr;
    switch (type) {
        case ASSIGNMENT:
            expr = vmalloc(sizeof(struct ast_assignment_t));
            break;

        case UNARY:
            expr = vmalloc(sizeof(struct ast_unary_t));
            break;

        case BINARY:
            expr = vmalloc(sizeof(struct ast_binary_t));
            break;

        case LITERAL:
            expr = vmalloc(sizeof(struct ast_literal_t));
            break;

        case PROGRAM_SEQUENCE:
            expr = vmalloc(sizeof(struct ast_program_sequence_t));

        case ENUM_COUNT:
            break;
    }

    expr->type = type;
    return expr;
}

/*
 *
program         ->      line EOF ;
 */
static void *ast_program(void)
{
    //void *line = ast_line();
    void *line = ast_pipe();
    consume(T_EOF, "error 100");
    return line;
}

static void *ast_line(void)
{
    return ast_unary();
}

/*
 * unary           ->      "!" unary
 *                 |        primary ;
 */
static void *ast_unary(void)
{
    if (match(1, T_BANG)) {
        struct token_t *op = previous();
        struct ast_node_t *right = ast_unary();

        struct ast_unary_t *expr = expr_malloc(UNARY);
        expr->op = op;
        expr->right = right;
        return expr;
    }

    return ast_assignment();
}

/*
 * assignment      ->      IDENTIFIER "=" assignment
 *                 |       primary ;
 */
static void *ast_assignment(void)
{
    if (match(1, T_IDENTIFIER)) {
        struct token_t *identifier = previous();
        if (match(1, T_EQUAL)) {
            struct ast_node_t *value = ast_assignment();
            struct ast_assignment_t *expr = expr_malloc(ASSIGNMENT);
            expr->name = identifier;
            expr->value = value;
            return expr;
        } else {
            // broken
            tokenlist->pos--;
        }
    }

    return ast_primary();
}

/*
 * primary         ->      NUMBER | STRING
 *                 |       IDENTIFIER ;
 */
static void *ast_primary(void)
{
    if (match(2, T_STRING, T_NUMBER)) {
        struct token_t *prev = previous();
        struct ast_literal_t *expr = expr_malloc(LITERAL);
        expr->type = prev->type;
        expr->literal = prev->literal;
        expr->literal_size = prev->literal_size;
        return expr;
    }
    return NULL;
}

/*
 * pipe            -> programSequence "|" programSequence
 *                 |  programSequence ;
 */
static void *ast_pipe(void)
{
    struct ast_node_t *left = ast_program_sequence();
    if (match(1, T_PIPE)) {
        struct token_t *op = previous();
        struct ast_node_t *right = ast_program_sequence();
        struct ast_binary_t *expr = expr_malloc(BINARY);
        expr->left = left;
        expr->op = op;
        expr->right = right;
        return expr;
    }

    return left;
}

/*
 * programSequence -> IDENTIFIER ( PRIMARY )* ;
 */
static void *ast_program_sequence(void)
{
    if (match(1, T_IDENTIFIER)) {
        struct token_t *program_name = previous();
        unsigned int argc = 0;

        //TODO: allocate dynamically
        struct ast_node_t **argv = vmalloc(sizeof(struct ast_node_t *) * 32);
        struct ast_node_t *arg;
        while (check_either(3, T_IDENTIFIER, T_NUMBER, T_STRING)) {
            arg = ast_primary();
            argv[argc++] = arg;
            if (argc == 32)
                valery_exit_parse_error("too many args");
        }

        struct ast_program_sequence_t *expr = expr_malloc(PROGRAM_SEQUENCE);
        expr->program_name = program_name;
        expr->argv = argv;
        expr->argc = argc;
        return expr;
    }

    return NULL;
}

struct ast_node_t *parse(struct tokenlist_t *tl)
{
    tokenlist = tl;

    /* start recursive descent */
    void *res = ast_program();
    return res;
}

void ast_free(struct ast_node_t *starting_node)
{
    // every expression type will ned their own free function
}
