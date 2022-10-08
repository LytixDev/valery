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

#include "valery/interpreter/lex.h"
#include "valery/interpreter/parser.h"
#include "valery/exec.h"


static void program_sequence(struct ast_program_sequence_t *expr);


static void pipe(struct ast_binary_t *expr)
{
    struct exec_ctx e_ctx = { .flags = SF_ADAM_VACANT | SF_SETH_VACANT, .read_stream = ST_NONE,
        .write_stream = ST_NONE };

    new_pipe(&e_ctx);

    // update exec flags

    //program_sequence((struct ast_program_sequence_t *)expr->left);
    struct ast_program_sequence_t *left = expr->left;
    struct ast_program_sequence_t *right = expr->right;
    char *argv[left->argc];
    #include "valery/env.h"
    #include "valery/load_config.h"
    struct env_t *env = env_malloc();
    parse_config(env->env_vars, env->paths);
    valery_exec_program(left->program_name->lexeme, argv, 0, env, &e_ctx);

    // pipe close write end
    e_ctx.read_stream = e_ctx.write_stream;
    e_ctx.write_stream = ST_NONE;

    if (e_ctx.read_stream == ST_ADAM)
        e_ctx.flags |= SF_ADAM_CLOSE;
    else
        e_ctx.flags |= SF_SETH_CLOSE;

    valery_exec_program(right->program_name->lexeme, argv, 0, env, &e_ctx);


    terminate_pipe(&e_ctx);
}


static void assignment(struct ast_assignment_t *expr)
{
}

static void unary(struct ast_unary_t *expr)
{
}

static void binary(struct ast_binary_t *expr)
{
    switch (expr->op->type) {
        case T_PIPE:
            pipe(expr);
            break;

        default:
            fprintf(stderr, "binary type not supported");
            break;
    }
}

static void literal(struct ast_literal_t *expr)
{
}

static void program_sequence(struct ast_program_sequence_t *expr)
{
}


static int interpret_node(ASTNodeHead *expr)
{
    switch (expr->type) {
        case ASSIGNMENT:
            assignment((struct ast_assignment_t *)expr);
            break;
        case UNARY:
            unary((struct ast_unary_t *)expr);
            break;
        case BINARY:
            binary((struct ast_binary_t *)expr);
            break;
        case LITERAL:
            literal((struct ast_literal_t *)expr);
            break;
        case PROGRAM_SEQUENCE:
            program_sequence((struct ast_program_sequence_t *)expr);
            break;

        case ENUM_COUNT:
            // ignore
            break;
    }

    return 0;
}

int interpret(ASTNodeHead *expr)
{
    interpret_node(expr);
    return 0;
}
