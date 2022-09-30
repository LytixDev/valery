#include "lex.h"


int main()
{
    char *source = "# comment this out\nls -la && echo \"hello world\"";
    Lex *lx = tokenize(source);
    lex_dump(lx);
}
