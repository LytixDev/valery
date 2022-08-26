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

#include <stdio.h>
#include <stdlib.h>

#include "valery/interpreter/parser.h"
#include "valery/interpreter/nlexer.h"


struct lex_t *lx_cpy;
static size_t current = 0;


static void parse_error(char *msg)
{
    fprintf(stderr, "parse error: %s\n", msg);
    exit(1);
}

/* architecture stuff */

static struct token_t peek()
{
    return *lx_cpy->tokens[current];
}

static bool eof()
{
    return peek().type == T_EOF;
}

static bool check(enum ttype_t type)
{
    if (eof()) return false;
    return peek().type == type;
}

static struct token_t previous()
{
    if (current == 0)
        parse_error("internal error, current = 0");
    return *lx_cpy->tokens[current - 1];
}

static struct token_t advance()
{
    if (!eof()) current++;
    return previous();
}

static bool match(enum ttype_t *types, size_t n)
{
    for (size_t i = 0; i < n; i++) {
        if (check(types[i])) {
            advance();
            return true;
        }
    }

    return false;
}

static struct token_t consume(enum ttype_t type, char *msg) {
    if (check(type)) return advance();
    parse_error(msg);
    exit(1);
}


expr_t *expression_v()
{
    return unary_v();
}

expr_t *unary_v()
{
    /*
     * unary           ->      "!" unary
     *                 |       primary ;
     */
    if (check(T_BANG)) {
        struct token_t operator_ = advance();
        expr_t *right = unary_v();
        return new_unary(operator_, right);
    }

    return primary_v();
};

expr_t *primary_v()
{
    /*
     *  primary         ->      NUMBER | STRING | "true" | "false"
     *                  |       ( "$(" | "(" ) expression ")" ;
     */

    enum ttype_t m[] = {T_NUMBER, T_STRING};
    if (match(m, 2))
        return new_literal(previous());
    //TODO: add true and false

    m[0] = T_DOLLAR_LPAREN;
    m[1] = T_LPAREN;

    if (match(m, 2)) {
        expr_t *expr = expression_v();
        consume(T_RPAREN, "expected ')' after expression");
        return new_grouping(expr);
    }

    return NULL;
};


expr_t *parse(struct lex_t *lx)
{
    lx_cpy = lx;
    /* start recursive descent */
    return expression_v();
}

expr_t *new_unary(struct token_t operator_, expr_t *right)
{
    struct unary_t *expr = malloc(sizeof(struct unary_t));
    expr->operator_ = operator_;
    expr->right = right;
    return expr;
}

expr_t *new_literal(struct token_t t)
{
    struct literal_t *expr = malloc(sizeof(struct literal_t));
    expr->literal = t.literal;
    expr->literal_size = t.literal_size;
    expr->type = t.type;
    return expr;
}

expr_t *new_grouping(expr_t *expression)
{
    struct grouping_t *expr = malloc(sizeof(struct grouping_t));
    expr->expr = expression;
    return expr;
}
