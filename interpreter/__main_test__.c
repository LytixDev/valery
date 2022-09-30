#include "lex.h"


int main()
{
    char *source = "&&!=!!!";
    Lex *lx = tokenize(source);
    lex_dump(lx);
}
