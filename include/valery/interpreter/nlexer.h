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

#ifndef VALERY_NLEXER_H
#define VALERY_NLEXER_H

#include <stddef.h>
#include <stdbool.h>


/* vars */

//const char *keywords[] = {
//    "do",       /* T_DO             */
//    "done",     /* T_DONE           */
//    "case",     /* T_CASE           */
//    "esac",     /* T_ESAC           */
//    "function", /* T_FUNCTION       */
//    "select",   /* T_SELECT         */
//    "until",    /* T_UNTIL          */
//    "if",       /* T_IF             */
//    "elif",     /* T_ELIF           */
//    "fi",       /* T_FI             */
//    "then",     /* T_THEN           */
//    "while",    /* T_WHILE          */
//    "else",     /* T_ELSE           */
//    "for",      /* T_FOR            */
//    "in",       /* T_IN             */
//    "time",     /* T_TIME           */
//    "return",   /* T_RETURN         */
//};

//const int keywords_len = 17;


/* types */

typedef enum ttype_t {
    /* single-character tokens. */
    T_LPAREN,
    T_RPAREN,
    T_LBRACE,
    T_RBRACE,
    T_COMMA,
    T_DOT,
    T_MINUS,
    T_PLUS,
    T_SEMICOLON,
    T_SLASH,
    T_STAR,

    /* one or two character tokens */
    T_BANG,
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

    /* literals */
    T_IDENTIFIER,
    T_STRING,
    T_NUMBER,
    T_BUILTIN,

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

    T_UNKNOWN,
    T_EOF
} ttype_t;


typedef struct token_t {
    enum ttype_t type;
    char *lexeme;
    void *literal;
    size_t literal_size;
    size_t line;
    //size_t lexeme_size;
    //size_t offset;
} token_t;


typedef struct lex_t {
    struct token_t **tokens;  /* list of the tokens */
    size_t size;              /* total tokens occupied */
    size_t capacity;          /* total tokens allocated */
} lex_t;


/* functions */

struct lex_t *lex_malloc(void);

void tokenize(struct lex_t *lx, char *source);

void scan_token(struct lex_t *lx);

void lex_dump(struct lex_t *lx);

void init_identifiers();

void destroy_identifiers();

#endif /* !VALERY_NLEXER_H */

