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
#include <stdio.h>

#include "lib/nicc/nicc.h"
#include "valery/interpreter/lexer.h"
#include "valery/interpreter/ast.h"

extern const char *tokentype_str[T_ENUM_COUNT][2];
static void ast_print_expr(struct Expr *expr_head);
static void ast_print_stmt(struct Stmt *stmt_head);

static void literal(struct LiteralExpr *expr)
{
    if (expr->value_type == LIT_STRING) {
        printf("%s", (char *)expr->value);
    } else if (expr->value_type == LIT_INT) {
        printf("%d", *(int *)expr->value);
    } else if (expr->value_type == LIT_DOUBLE) {
        printf("%lf", *(double *)expr->value);
    } else {
        struct darr_t *expansion = expr->value;

        size_t len = darr_get_size(expansion);
        for (size_t i = 0; i < len; i++) {
            struct expansion_t *e = darr_get(expansion, i);
            if (e->type == ET_LITERAL)
                printf("%s", (char *)e->value);
            else if (e->type == ET_PARAMETER)
                printf("[%s]", (char *)e->value);
            else
                printf("literal type not supported in AST print");
        }
    }
}

static void command(struct CommandExpr *expr)
{
    int bound = darr_get_size(expr->exprs);
    for (int i = 0; i < bound; i++) {
        ast_print_expr(darr_get(expr->exprs, i));
        if (i != bound - 1)
            putchar(' ');
    }
}

static void binary(struct BinaryExpr *expr)
{
    ast_print_expr(expr->left);
    if (expr->operator_ == NULL)
        return;
    printf(" %s ", tokentype_str[expr->operator_->type][0]);
    ast_print_expr(expr->right);
}

static void variable(struct VariableExpr *expr)
{
    printf("$%s", expr->name->lexeme);
}

static void var_stmt(struct VarStmt *stmt)
{
    printf("%s = ", stmt->name->lexeme);
    ast_print_expr(stmt->initializer);
}

static void ast_print_expr(struct Expr *expr_head)
{
    if (expr_head == NULL)
        return;

    switch (expr_head->type) {
        case EXPR_BINARY:
            binary((struct BinaryExpr *)expr_head);
            break;
        case EXPR_COMMAND:
            command((struct CommandExpr *)expr_head);
            break;
        case EXPR_LITERAL:
            literal((struct LiteralExpr *)expr_head);
            break;
        case EXPR_VARIABLE:
            variable((struct VariableExpr *)expr_head);
            break;

        default:
            printf("AST TYPE NOT HANLDED, %d\n", expr_head->type);
    }
}

static void ast_print_stmt(struct Stmt *stmt_head)
{
    if (stmt_head == NULL)
        return;

    putchar('{');

    switch (stmt_head->type) {
        case STMT_EXPRESSION:
            ast_print_expr(((struct ExpressionStmt *)stmt_head)->expression);
            break;
        case STMT_VAR:
            var_stmt((struct VarStmt *)stmt_head);
            break;
        default:
            printf("AST TYPE NOT HANLDED, %d\n", stmt_head->type);
    }

    putchar('}');
}

void ast_print(struct darr_t *statements)
{
    printf("\n--- AST dump ---\n");
    for (size_t i = 0; i < darr_get_size(statements); i++) {
        ast_print_stmt(darr_get(statements, i));
        putchar('\n');
    }
}
