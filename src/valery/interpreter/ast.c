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
#include <stdio.h>

#include "lib/nicc/nicc.h"
#include "valery/interpreter/lexer.h"
#include "valery/interpreter/ast.h"

extern const char *tokentype_str[T_ENUM_COUNT];
static void ast_print_node(ASTNodeHead *expr);

static void unary_print(struct UnaryExpr *expr)
{
    putchar('(');
    //printf("%s=%s", tokentype_str[expr->head.token->type], expr->head.token->lexeme);
    ast_print_node(expr->right);
    putchar(')');
}

static void binary_print(struct BinaryExpr *expr)
{
    putchar('(');
    ast_print_node(expr->left);
    //printf("%s=%s", tokentype_str[expr->head.token->type], expr->head.token->lexeme);
    //printf("%s", tokentype_str[expr->head.token->type]);
    ast_print_node(expr->right);
    putchar(')');
}

static void list_print(struct ListExpr *expr)
{
    struct token_t *token;
    putchar('(');
    //printf("LIST: %s:", expr->head.token->lexeme);
    //for (size_t i = 0; i < darr_get_size(expr->argv); i++) {
    //    token = darr_get(expr->argv, i);
    //    printf(" %s", token->lexeme);
    //}
    putchar(')');
}

static void ast_print_node(ASTNodeHead *expr)
{
    if (expr == NULL)
        return;

    switch (expr->type) {
        case AST_UNARY:
            unary_print((struct UnaryExpr *)expr);
            break;
        case AST_BINARY:
            binary_print((struct BinaryExpr *)expr);
            break;
        case AST_LIST:
            list_print((struct ListExpr *)expr);
            break;

        default:
            printf("AST TYPE NOT HANLDED, %d\n", expr->type);
    }
}

void ast_print(ASTNodeHead *expr)
{
    printf("\n--- AST dump ---\n");
    ast_print_node(expr);
    putchar('\n');
}
