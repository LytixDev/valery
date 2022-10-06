/*
 *  Copyright (C) 2022 Nicolai Brand 
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
 *  You should have received a copy of the GNU General Public License along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef VALERY_INTERPRETER_LEX_H
#define VALERY_INTERPRETER_LEX_H
#include <stddef.h>             // size_t type


/* types */

#define KEYWORDS_LEN 17
enum tokentype_t {
    /* keywords */
    T_DO,
    T_DONE,
    T_CASE,
    T_ESAC,
    T_FUNCTION,
    T_SELECT,
    T_UNTIL,
    T_IF,
    T_ELIF,
    T_FI,
    T_THEN,
    T_WHILE,
    T_ELSE,
    T_FOR,
    T_IN,
    T_TIME,
    T_RETURN,

    /* single-character tokens */
    T_LPAREN,
    T_RPAREN,
    T_LBRACE,
    T_RBRACE,
    T_COMMA,
    T_MINUS,
    T_PLUS,
    T_COLON,
    T_SEMICOLON,
    T_SLASH,
    T_STAR,

    /* one or two character tokens */
    T_DOLLAR,
    T_DOLLAR_LPAREN,
    T_ANP,
    T_ANP_ANP,
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

    /* literals */
    T_IDENTIFIER,
    T_STRING,
    T_NUMBER,

    T_NEWLINE,
    T_UNKNOWN,
    T_EOF,
    T_ENUM_COUNT        /* not an actual type */
};

struct token_t {
    enum tokentype_t type;
    //TODO: union mayhaps/perchance?
    char *lexeme;
    void *literal;
    size_t literal_size;
    //size_t line;
    //size_t lexeme_size;
    //size_t offset;
};

struct tokenlist_t {
    struct token_t **tokens;           /* list of the tokens */
    size_t size;              /* total tokens occupied */
    size_t capacity;          /* total tokens allocated */
};


//const char *tokentype_str[T_ENUM_COUNT];


/* functions */
struct tokenlist_t *tokenize(char *source);

void token_list_dump(struct tokenlist_t *tl);


#endif /* !VALERY_INTERPRETER_LEX_H */
