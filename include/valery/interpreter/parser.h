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

#include "lex.h"        // struct tokenlist_t type

/* types */

enum ast_type_t {
    ASSIGNMENT,
    UNARY,
    BINARY,
    LITERAL,
    PROGRAM_SEQUENCE,
    ENUM_COUNT
};

typedef struct ast_node_head_t {
    enum ast_type_t type;
} ASTNodeHead;


/* expression types */

/*
 * every expression starts with an ASTNodeHead describing its type.
 */
typedef ASTNodeHead Expr;

struct ast_assignment_t {
    ASTNodeHead head;
    struct token_t *name;
    Expr *value;
};

struct ast_unary_t {
    ASTNodeHead head;
    struct token_t *op;  // operator
    Expr *right;
};

struct ast_binary_t {
    ASTNodeHead head;
    Expr *left;
    struct token_t *op;
    Expr *right;
};

struct ast_literal_t {
    ASTNodeHead head;
    enum tokentype_t type;
    void *literal;
    size_t literal_size;
};

struct ast_program_sequence_t {
    ASTNodeHead head;
    struct token_t *program_name;
    Expr **argv;
    unsigned int argc;
};


/* functions */
Expr *parse(struct tokenlist_t *tl);

void ast_free(Expr *starting_node);

void ast_print(ASTNodeHead *first);


#endif /* !VALERY_INTERPRETER_PARSER_H */