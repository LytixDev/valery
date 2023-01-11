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

#ifndef VALERY_INTERPRETER_AST_H
#define VALERY_INTERPRETER_AST_H

#include "lexer.h"              // struct tokenlist_t type
//#ifndef NICC_IMPLEMENTATION
//#       define NICC_IMPLEMENTATION
//#endif
#include "lib/nicc/nicc.h"      // dynamic array (darr_t)

/* types */
enum ast_type_t {
    AST_UNARY,
    AST_BINARY,
    AST_LITERAL,
    AST_LIST,
    ENUM_COUNT
};

typedef struct ast_node_t {
    enum ast_type_t type;
    //struct token_t *token;
} ASTNodeHead;

/* 
 * expression types.
 * every expression starts with an ASTNodeHead describing its type.
 */
struct UnaryExpr {
    ASTNodeHead head;
    struct token_t *this;
    struct ast_node_t *right;
};

struct BinaryExpr {
    ASTNodeHead head;
    struct token_t *this;
    struct ast_node_t *left;
    struct ast_node_t *right;
};

struct LiteralExpr {
    ASTNodeHead head;
    struct token_t *this;
};

struct ListExpr {
    ASTNodeHead head;
    struct darr_t *exprs;       /* dynamic array of ast nodes */
};


/* functions */
void ast_free(struct ast_node_t *starting_node);

void ast_print(ASTNodeHead *first);

#endif /* !VALERY_INTERPRETER_AST_H */
