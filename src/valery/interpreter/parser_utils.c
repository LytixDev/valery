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
#include <stdbool.h>                    // bool type
#include <stdarg.h>                     // va_start, va_arg, va_end 

#include "valery/interpreter/ast.h"
#include "valery/interpreter/parser_utils.h"
#define SAC_IMPLEMENTATION
#include "lib/sac/sac.h"
#include "lib/nicc/nicc.h"

extern struct tokenlist_t *tokenlist;   // defined in parser.c, TODO: globals are le bad
struct m_arena *ast_arena = NULL;       // the memory arena to alloc abstract syntax tree nodes onto
                                        // ex: Stmt or Expr

bool check_single(enum tokentype_t type)
{
    if (tokenlist->size >= tokenlist->capacity)
        return false;

    return tokenlist->tokens[tokenlist->pos]->type == type;
}

struct token_t *previous()
{
    return tokenlist->tokens[tokenlist->pos - 1];
}

bool check_either(unsigned int n, ...)
{
    enum tokentype_t type;
    va_list args;
    va_start(args, n);

    for (unsigned int i = 0; i < n; i++) {
        type = va_arg(args, enum tokentype_t);
        if (check_single(type))
            return true;
    }

    va_end(args);
    return false;
}

bool match_either(unsigned int n, ...)
{
    enum tokentype_t type;
    va_list args;
    va_start(args, n);

    for (unsigned int i = 0; i < n; i++) {
        type = va_arg(args, enum tokentype_t);
        if (check_single(type)) {
            tokenlist->pos++;
            return true;
        }
    }

    va_end(args);
    return false;
}

void *consume(enum tokentype_t type, char *err_msg)
{
    if (!check_single(type))
        valery_exit_parse_error(err_msg);

    return tokenlist->tokens[tokenlist->pos++];
}

struct Expr *expr_alloc(enum ExprType type, struct token_t *token)
{
    struct Expr *expr;
    switch (type) {
        case EXPR_UNARY:
            expr = m_arena_alloc(ast_arena, sizeof(struct UnaryExpr));
            break;

        case EXPR_BINARY:
            expr = m_arena_alloc(ast_arena, sizeof(struct BinaryExpr));
            break;

        case EXPR_LITERAL:
            expr = m_arena_alloc(ast_arena, sizeof(struct LiteralExpr));
            ((struct LiteralExpr *)expr)->value = token->literal;
            if (token->type == T_WORD || token->type == T_STRING)
                ((struct LiteralExpr *)expr)->value_type = LIT_STRING;
            if (token->type == T_WORD || token->type == T_STRING)
                ((struct LiteralExpr *)expr)->value_type = LIT_STRING;
            else
                ((struct LiteralExpr *)expr)->value_type = LIT_INT;
            break;

        case EXPR_COMMAND:
            expr = m_arena_alloc(ast_arena, sizeof(struct CommandExpr));
            ((struct CommandExpr *)expr)->exprs = darr_malloc();   /* TODO: put on arena */
            break;
    }

    expr->type = type;
    return expr;
}

struct Stmt *stmt_alloc(enum StmtType type, struct token_t *token)
{
    struct Stmt *stmt;
    switch (type) {
        case STMT_EXPRESSION:
            stmt = m_arena_alloc(ast_arena, sizeof(struct ExpressionStmt));
            break;
    }
    stmt->type = type;
    return stmt;
}


void ast_arena_init()
{
    ast_arena = m_arena_init(GB_SIZE_T(16), 4096);
}

void ast_arena_clear()
{
    m_arena_clear(ast_arena);
}

void ast_arena_release()
{
    m_arena_release(ast_arena);
}
