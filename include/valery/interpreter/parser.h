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

#define expr_t void

/* types */

typedef struct binary_t {
    expr_t *left;
    enum ttype_t token;
    expr_t *right;
    void *(*visit)(struct binary_t *);
} binary_t;

typedef struct grouping_t {
    expr_t *expr;
    void *(*visit)(struct grouping_t *);
} grouping_t;

typedef struct literal_t {
    void *literal;
    size_t literal_size;
    ttype_t type; // determines the type of the void *
    void *(*visit)(struct literal_t *);
} literal_t;

typedef struct unary_t {
    struct token_t operator_;
    expr_t *right;
    void *(*visit)(struct unary_t *);
} unary_t;


/* functions */

expr_t *parse(struct lex_t *lx);

expr_t *expression_v();

expr_t *unary_v();

expr_t *primary_v();

expr_t *new_unary(struct token_t operator_, expr_t *right);

expr_t *new_literal(struct token_t t);

expr_t *new_grouping(expr_t *expression);

#endif /* VALERY_PARSER_H */
