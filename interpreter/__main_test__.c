#include "lex.h"
#include "parser.h"


int main()
{
    char *source = "# comment this out\necho \"hello world\" && exit 1";
    struct tokenlist_t *lx = tokenize(source);
    token_list_dump(lx);

    Stmt *statement = parse(lx);
}
