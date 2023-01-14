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
enum AstExprType {
    EXPR_UNARY,
    EXPR_BINARY,
    EXPR_LITERAL,
    EXPR_COMMAND,
    EXPR_ENUM_COUNT
};

enum AstStmtType {
    STMT_IF,
    STMT_EXPRESSION,
    STMT_ENUM_COUNT
};

struct AstNodeHead {
    union {
        enum AstExprType expr_type;
        enum AstStmtType stmt_type;
    };
    //struct token_t *token;
};

/* 
 * expression types.
 * every expression starts with an struct AstNodeHead describing its type.
 */
struct UnaryExpr {
    struct AstNodeHead head;
    struct token_t *operator_;
    struct AstNodeHead *right;
};

struct BinaryExpr {
    struct AstNodeHead head;
    struct AstNodeHead *left;
    struct token_t *operator_;
    struct AstNodeHead *right;
};

struct LiteralExpr {
    struct AstNodeHead head;
    void *value;
};

struct CommandExpr {
    struct AstNodeHead head;
    struct darr_t *exprs;       /* dynamic array of ast nodes */
};

struct VariableExpr {
    struct AstNodeHead head;
    struct token_t *name;
};

struct ExpressionStmt {
    struct AstNodeHead head;
    struct AstNodeHead *expression;
};

struct IfStmt {
    struct AstNodeHead head;
    struct AstNodeHead *condition;
    struct AstNodeHead *then_branch;
    struct AstNodeHead *else_branch;

};


/* functions */
void ast_free(struct AstNodeHead *starting_node);

void ast_print(struct AstNodeHead *first);

#endif /* !VALERY_INTERPRETER_AST_H */
