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
    struct tokenlist_t *tl = tokenize(source);
    tokenlist_dump(tl);

    ASTNodeHead *expr = parse(tl);
    ast_print(expr);
    //int rc = interpret(expr);
    //tokenlist_free(tl);
    ast_arena_release();
}
