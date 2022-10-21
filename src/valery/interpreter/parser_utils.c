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
 *  You should have received a copy of the GNU General Public License along with this program.
 *  If not, see <https://www.gnu.org/licenses/>.
 */
#include <stdbool.h>                    // bool type
#include <stdarg.h>                     // va_start, va_arg, va_end 

#include "valery/interpreter/parser_utils.h"
#include "valery/interpreter/parser.h"  // ast_type_t
#include "valery/interpreter/lexer.h"   // struct tokenlist_t type
#include "valery/valery.h"              // errors

extern struct tokenlist_t *tokenlist;   // defined in parser.c


bool check(enum tokentype_t type)
{
    if (tokenlist->size >= tokenlist->capacity)
        return false;

    return tokenlist->tokens[tokenlist->pos]->type == type;
}

struct token_t *previous()
{
    return tokenlist->tokens[tokenlist->pos - 1];
}


/*
 * @returns true if next token is any of the given arguments, else false.
 */
bool check_either(unsigned int n, ...)
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
bool match_either(unsigned int n, ...)
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

void consume(enum tokentype_t type, char *err_msg)
{
    if (!check(type))
        valery_exit_parse_error(err_msg);

    tokenlist->pos++;
}

/*
 * allocates space for the given expression type.
 * sets the newly allocated expression's type to the given type.
 */
void *expr_malloc(enum ast_type_t type)
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
    }

    expr->type = type;
    return expr;
}
