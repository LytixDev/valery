#include <stdio.h>

#include "valery/interpreter/nlexer.h"
#include "valery/interpreter/parser.h"


int main()
{
    init_identifiers();


    char *buffer = "!\"hello world!\"";
    struct lex_t *lx = lex_malloc();
    tokenize(lx, buffer);

#ifdef DEBUG
    lex_dump(lx);
#endif

    expr_head *expr = parse(lx);
    struct unary_t *a = (struct unary_t *)expr;
    printf("%s\n", (char*)((struct literal_t *)a->right)->literal);

    destroy_identifiers();
}

