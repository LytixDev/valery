#include "lex.h"
#include "parser.h"


int main()
{
    char *source = "!\"hello world\"";
    struct tokenlist_t *lx = tokenize(source);
    token_list_dump(lx);

    Expr *expr = parse(lx);
    ast_print(expr);
}
