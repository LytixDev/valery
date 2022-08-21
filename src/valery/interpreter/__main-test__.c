#include <stdio.h>

#include "valery/interpreter/nlexer.h"


int main()
{
    init_keywords();

    char *buffer = "(===#hey\n) \"item crate\" 10";
    struct lex_t *lx = lex_malloc();
    tokenize(lx, buffer);
}

