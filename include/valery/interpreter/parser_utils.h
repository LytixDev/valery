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

#ifndef VALERY_INTERPRETER_PARSER_UTILS_H
#define VALERY_INTERPRETER_PARSER_UTILS_H

#include <stdbool.h>

#include "valery/valery.h"              // VA_NUMBER_OF_ARGS
#include "valery/interpreter/parser.h"  // tokentype_t
#include "valery/interpreter/parser.h"  // expr_type_t

/* functions */
bool check_single(enum tokentype_t type);

struct token_t *previous();

/*
 * @returns true if next token is any of the given arguments, else false.
 */
bool check_either(unsigned int n, ...);
#define check(...) check_either(VA_NUMBER_OF_ARGS(__VA_ARGS__), __VA_ARGS__)

/*
 * consumes the next token if its type is one of the given arguments.
 * @returns true if successfully consumed a token, else false.
 */
bool match_either(unsigned int n, ...);
#define match(...) match_either(VA_NUMBER_OF_ARGS(__VA_ARGS__), __VA_ARGS__)

/*
 * consumes the given token.
 * gives error if next token does not have the passed in type .
 * @returns the consumed token.
 */
void *consume(enum tokentype_t type, char *err_msg);

/*
 * allocates space for the given expression type.
 * sets the newly allocated expression's type to the given type.
 */
void *expr_alloc(enum AstExprType type, struct token_t *token);


void ast_arena_init();
void ast_arena_clear();
void ast_arena_release();

#endif /* VALERY_INTERPRETER_PARSER_UTILS_H */
