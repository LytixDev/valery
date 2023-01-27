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

#ifndef VALERY_INTERPRETER_AST_H
#define VALERY_INTERPRETER_AST_H

#include "lexer.h"
#include "lib/nicc/nicc.h"

/* types */
enum ExprType {
    EXPR_UNARY,
    EXPR_BINARY,
    EXPR_LITERAL,
    EXPR_COMMAND,
    EXPR_VARIABLE,
    EXPR_ENUM_COUNT
};

enum StmtType {
    STMT_IF,
    STMT_EXPRESSION,
    STMT_VAR,
    STMT_ENUM_COUNT
};

/* a literal is either a string, int or float */
enum LiteralType { LIT_STRING, LIT_INT, LIT_FLOAT };


/*
 * evil trick to get some sort of polymorphism (I concede, my brain has been corrupted by OOP)
 */
struct Expr {
    enum ExprType type;
};

struct Stmt {
    enum StmtType type;
};

/* expressions */
struct UnaryExpr {
    struct Expr head;
    struct token_t *operator_;
    struct Expr *right;
};

struct BinaryExpr {
    struct Expr head;
    struct Expr *left;
    struct token_t *operator_;
    struct Expr *right;
};

struct LiteralExpr {
    struct Expr head;
    void *value;
    enum LiteralType value_type; //TODO: ugly? could use a tagged union
};

struct CommandExpr {
    struct Expr head;
    struct darr_t *exprs;       /* dynamic array of ast nodes */
};

struct VariableExpr {
    struct Expr head;
    struct token_t *name;
};


/* statements */
struct ExpressionStmt {
    struct Stmt head;
    struct Expr *expression;
};

struct IfStmt {
    struct Stmt head;
    struct Expr *condition;
    struct Stmt *then_branch;
    struct Stmt *else_branch;

};

struct VarStmt {
    struct Stmt head;
    struct token_t *name;
    struct Expr *initializer;
};


/* functions */
/*
 * recursively prints the abstract syntax tree
 */
void ast_print(struct darr_t *statements);

#endif /* !VALERY_INTERPRETER_AST_H */
