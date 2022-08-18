#include <stdio.h>

#include "valery/interpreter/nlexer.h"


int main()
{
    char *buffer = "|| if && &";
    struct lex_t *lx = lex_malloc();
    tokenize(lx, buffer);
}

