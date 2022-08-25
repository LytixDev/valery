#include <stdio.h>

#include "valery/interpreter/nlexer.h"
#include "valery/interpreter/parser.h"


int main()
{
    init_identifiers();


    char *buffer = "$(ls) && exit";
    struct lex_t *lx = lex_malloc();
    tokenize(lx, buffer);

#ifdef DEBUG
    lex_dump(lx);
#endif

    parse(lx);


    destroy_identifiers();
}

