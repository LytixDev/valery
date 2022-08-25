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

static struct token_t peek() {
    return *lx_cpy->tokens[current];
}

static bool eof() {
    return peek().type == T_EOF;
}

static bool check(enum ttype_t type) {
    if (eof()) return false;
    return peek().type == type;
}

static struct token_t previous() {
    if (current == 0)
        parse_error("internal error, current = 0");
    return *lx_cpy->tokens[current - 1];
}

static struct token_t advance() {
    if (!eof()) current++;
    return previous();
}

static bool match(enum ttype_t *types, size_t n) {
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
        struct token_t operator = advance();
        expr_t *right = unary_v();
        return NULL;
    }

    return primary_v();
};

expr_t *primary_v()
{
    return NULL;
};


expr_t *parse(struct lex_t *lx)
{
    lx_cpy = lx;
    /* start recursive descent */
    return expression_v();
}

