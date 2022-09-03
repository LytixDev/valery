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
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */


#ifndef VALERY_PARSER_H
#define VALERY_PARSER_H

#include "valery/interpreter/nlexer.h"
#include <stdlib.h>

//#define expr_t void

/* types */

typedef enum expr_type {
    E_BINARY,
    E_GROUPING,
    E_LITERAL,
    E_UNARY
} expr_type;

typedef struct expr_head {
    expr_type e_type;
    void *(*visit)(struct expr_head *);
} expr_head;

typedef struct binary_t {
    expr_head *head;
    expr_head *left;
    enum ttype_t token;
    expr_head *right;
} binary_t;

typedef struct grouping_t {
    expr_head *head;
    expr_head *expr;
} grouping_t;

typedef struct literal_t {
    expr_head *head;
    void *literal;
    size_t literal_size;
    ttype_t type; // determines the type of the void *
} literal_t;

typedef struct unary_t {
    expr_head *head;
    struct token_t operator_;
    expr_head *right;
} unary_t;


/* functions */

expr_head *parse(struct lex_t *lx);

expr_head *expression_v();

expr_head *unary_v();

expr_head *primary_v();

expr_head *new_unary(struct token_t operator_, expr_head *right);

expr_head *new_literal(struct token_t t);

expr_head *new_grouping(expr_head *expression);

#endif /* VALERY_PARSER_H */
