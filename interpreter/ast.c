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
#include <stdio.h>

#include "lex.h"
#include "parser.h"

extern const char *tokentype_str[T_ENUM_COUNT];
static void ast_print_node(ASTNodeHead *expr);


static void unary_print(struct ast_unary_t *expr)
{
    putchar('(');

    printf("unary -> ");
    printf("op: %s ", tokentype_str[expr->op->type]);
    ast_print_node(expr->right);

    putchar(')');
}

static void binary_print(struct ast_binary_t *expr)
{
    putchar('(');

    printf("binary -> ");
    ast_print_node(expr->left);
    printf("op: %s ", tokentype_str[expr->op->type]);
    ast_print_node(expr->right);

    putchar(')');
}

static void literal_print(struct ast_literal_t *expr)
{
    putchar('(');

    printf("literal -> ");
    if (expr->type == T_STRING) {
        printf("%s", (char *)expr->literal);
    } else if (expr->type == T_NUMBER) {
        printf("%d", *(int *)expr->literal);
    }

    putchar(')');
}

static void ast_print_node(ASTNodeHead *expr)
{
    switch (expr->type) {
        case UNARY:
            unary_print((struct ast_unary_t *)expr);
            break;
        case BINARY:
            binary_print((struct ast_binary_t *)expr);
            break;
        case LITERAL:
            literal_print((struct ast_literal_t *)expr);
            break;

        default:
            printf("AST TYPE NOT HANLDED\n");
    }
}

void ast_print(ASTNodeHead *expr)
{
    printf("\n--- AST dump ---\n");
    ast_print_node(expr);
    putchar('\n');
}
