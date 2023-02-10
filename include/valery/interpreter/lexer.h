/*
 *  Copyright (C) 2022-2023 Nicolai Brand 
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along with this program.
 *  If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef VALERY_INTERPRETER_LEX_H
#define VALERY_INTERPRETER_LEX_H
#include <stddef.h>


/* types */

#define KEYWORDS_LEN 14
enum tokentype_t {
    /* keywords */
    T_IF,
    T_THEN,
    T_ELSE,
    T_ELIF,
    T_FI,
    T_DO,
    T_DONE,
    T_CASE,
    T_ESAC,
    T_WHILE,
    T_UNTIL,
    T_FOR,
    T_RETURN,
    T_IN,

    /* single-character tokens */
    T_LPAREN,
    T_RPAREN,
    T_LBRACE,
    T_RBRACE,
    T_SEMICOLON,
    T_STAR,
    T_DOLLAR,

    /* one or two character tokens */
    T_ANP,
    T_AND_IF,
    T_BANG,
    T_BANG_BANG,
    T_BANG_EQUAL,
    T_EQUAL,
    T_EQUAL_EQUAL,
    T_GREATER,
    T_GREATER_EQUAL,
    T_LESS,
    T_LESS_EQUAL,
    T_LBRACKET,
    T_LBRACKET_LBRACKET,
    T_RBRACKET,
    T_RBRACKET_RBRACKET,
    T_DOT,
    T_DOT_DOT,
    T_PIPE,
    T_PIPE_PIPE,

    /* symbols/identifiers */
    T_IDENTIFIER,
    T_WORD,
    T_ASSIGNMENT_WORD,
    T_NAME,
    T_NEWLINE,
    IO_NUMBER,
    T_EXPANSION,
    T_NUMBER,

    T_UNKNOWN,
    T_EOF,
    T_ENUM_COUNT        /* not an actual type */
};

struct token_t {
    enum tokentype_t type;
    //TODO: union between lexeme and expansion?
    char *lexeme;
    struct darr_t *expansions;
};

/*
 * words and strings may be parameter expanded by the $, ~, or . symbol.
 * in the example:
 *
 *      a=hello
 *      echo "$a world!"
 *
 * "$a world!" needs to be expanded into "hello world!"
 * the internal lexical representation for the string looks like:
 * expansionlist
 *      1. type: ET_EXPAND, str -> 'a'
 *      2. type: ET_LITERAL, str -> ' word!'
 *
 *
 * similarly:
 *
 *      echo "$(ls -la | wc -l) files present"
 *
 * $(ls -la | wc -l) needs to be expanded into the result of the subshell
 * the internal lexical representation for the string looks like:
 * expansionlist
 *      1. type: ET_TOKENLIST, tl -> T_WORD (ls) T_WORD (-la) T_PIPE T_WORD (wc) T_WORD (-l)
 *      2. type: ET_LITERAL, str -> ' files present'
 */
enum expansion_type { ET_LITERAL, ET_VAR_EXPAND, ET_SUBSHELL };

struct expansion_t {
    enum expansion_type type;
    void *value;
    //union {
    //    char *str;
    //    struct darr_t *tokens;
    //};
};

/* functions */
/*
 * performs a lexical analysis on the given source code
 * @returns a list of tokens
 */
struct darr_t *tokenize(char *source);

/*
 * prints the tokens in sequential order as they appear in the list
 */
void tokenlist_print(struct darr_t *tokens);


#endif /* !VALERY_INTERPRETER_LEX_H */
