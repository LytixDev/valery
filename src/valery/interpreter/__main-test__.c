#include <stdio.h>

#include "valery/interpreter/nlexer.h"
#include "valery/interpreter/parser.h"


int main()
{
    init_identifiers();


    char *buffer = "10";
    struct lex_t *lx = lex_malloc();
    tokenize(lx, buffer);

#ifdef DEBUG
    lex_dump(lx);
#endif

    expr_t *expr = parse(lx);
    struct literal_t *l = (struct literal_t *)expr;
    printf("%d\n", *(int *)l->literal);

    destroy_identifiers();
}

