#include "lex.h"
#include "parser.h"


int main()
{
    char *source = "# comment this out\necho \"hello world\" && exit 1";
    TokenList *lx = tokenize(source);
    token_list_dump(lx);

    Stmt *statement = parse(lx);
}
