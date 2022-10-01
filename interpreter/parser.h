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

#ifndef VALERY_INTERPRETER_PARSER_H
#define VALERY_INTERPRETER_PARSER_H

#include "lex.h"        // TokenList type

/* types */
typedef struct ast_node_head_t {
    int type;
} ASTNodeHead;

typedef struct expr_t Expr;
typedef struct stmt_t Stmt;


/* functions */
void *parse(TokenList *tl);

void ast_print(Stmt *first);

#endif /* !VALERY_INTERPRETER_PARSER_H */
