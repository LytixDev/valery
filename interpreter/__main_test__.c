#include "lex.h"
#include "parser.h"


int main()
{
    char *source = "ls \"my_file\" 10 | wc";
    struct tokenlist_t *tl = tokenize(source);
    tokenlist_dump(tl);

    Expr *expr = parse(tl);
    ast_print(expr);
    tokenlist_free(tl);
}
