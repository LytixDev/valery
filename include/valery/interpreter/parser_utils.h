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

#include "valery/interpreter/parser.h"  // tokentype_t
#include "valery/interpreter/parser.h"  // ast_type_t

/* functions */
bool check(enum tokentype_t type);

struct token_t *previous();

/*
 * @returns true if next token is any of the given arguments, else false.
 */
bool check_either(unsigned int n, ...);

/*
 * consumes the next token if its type is one of the given arguments.
 * @returns true if successfully consumed a token, else false.
 */
bool match_either(unsigned int n, ...);

void consume(enum tokentype_t type, char *err_msg);

/*
 * allocates space for the given expression type.
 * sets the newly allocated expression's type to the given type.
 */
void *expr_malloc(enum ast_type_t type);

#endif /* VALERY_INTERPRETER_PARSER_UTILS_H */