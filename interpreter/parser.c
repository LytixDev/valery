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

static void *ast_program()
{
    void *line = ast_line();
    consume(T_EOF, "error 100");
    return line;
}

static void *ast_line()
{
    //TODO: ugly
    for (struct token_t current = get_current_token(); current.type != T_EOF;) {
    }
    return NULL;
}

static void *ast_primary()
{
    return NULL;
}

void *parse(struct tokenlist_t *tl)
{
    tl_cpy = tl;

    /* start recursive descent */
    void *res = ast_program();


    return NULL;
}
