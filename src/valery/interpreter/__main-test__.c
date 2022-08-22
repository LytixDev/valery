#include <stdio.h>

#include "valery/interpreter/nlexer.h"


int main()
{
    init_identifiers();

    char *buffer = "if a==b ";
    struct lex_t *lx = lex_malloc();
    tokenize(lx, buffer);
}

