#include <stdio.h>

#include "valery/interpreter/parser_utils.h"
#include "valery/valery.h"
#include "valery/interpreter/ast.h"
#include "valery/interpreter/lexer.h"
#include "valery/interpreter/parser.h"
#include "valery/interpreter/interpreter.h"

int main()
{
    char source[1024];
    FILE *fp = fopen("src/test.sh", "r");
    if (fp == NULL)
        valery_exit_internal_error("could not open file test.sh :-(");

    int c;
    int i = 0;
    do {
        c = fgetc(fp);
        source[i++] = c;
        if (i == 1024)
            break;
    } while (c != EOF);

    source[--i] = 0;

    ast_arena_init();
    printf("%s\n", source);
    struct darr_t *tokens = tokenize(source);
    tokenlist_print(tokens);

    struct darr_t *stmts = parse(tokens);
    ast_print(stmts);

    //int rc = interpret(stmts);
    //tokenlist_free(tokens);
    ast_arena_release();
}
