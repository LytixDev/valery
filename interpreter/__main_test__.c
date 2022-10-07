#include "lex.h"
#include "parser.h"


int main()
{
    char *source = "ls \"my_file\" 10 | wc";
    struct tokenlist_t *lx = tokenize(source);
    token_list_dump(lx);

    Expr *expr = parse(lx);
    ast_print(expr);
}
