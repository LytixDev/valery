#include "lex.h"
#include "parser.h"
#include "interpreter.h"


int main()
{
    char *source = "ls \"my_file\" 10 | wc";
    struct tokenlist_t *tl = tokenize(source);
    tokenlist_dump(tl);

    ASTNodeHead *expr = parse(tl);
    ast_print(expr);

    int rc = interpret(expr);

    tokenlist_free(tl);
    ast_free(expr);
}
