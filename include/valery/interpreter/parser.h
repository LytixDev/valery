/*
 *  Copyright (C) 2022-2023 Nicolai Brand 
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

#ifndef VALERY_INTERPRETER_PARSER_H
#define VALERY_INTERPRETER_PARSER_H

#include "valery/interpreter/ast.h"
#include "valery/interpreter/lexer.h"

/* functions */
/*
 * parses a list of tokens into a list of statements.
 * the statements are the first nodes in an abstract syntax tree representation of the semantics
 * @returns a dynamic list of struct Stmt
 */
struct darr_t *parse(struct darr_t *tokens);

#endif /* !VALERY_INTERPRETER_PARSER_H */
