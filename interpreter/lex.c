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
#include <stdlib.h>     // malloc, free
#include <string.h>     // strlen

#include "lex.h"
#define NICC_HT_IMPLEMENTATION
#include "nicc/nicc.h"  // hashtable implementation


/* types */
#define KEYWORDS_LEN 17
typedef enum ttype_t {
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

    T_UNKNOWN,
    T_EOF
} TokenType;

struct token_t {
    enum ttype_t type;
    char *lexeme;
    void *literal;
    size_t literal_size;
    size_t line;
    //size_t lexeme_size;
    //size_t offset;
};

struct lex_t {
    Token **tokens;           /* list of the tokens */
    size_t size;              /* total tokens occupied */
    size_t capacity;          /* total tokens allocated */
};


/* globals */
static struct ht_t *identifiers = NULL;

/* functions */

/* fills the global hashtable 'identifiers' with the supported identifiers */
static void init_identifiers()
{
    if (identifiers != NULL)
        return;

    identifiers = ht_malloc(32);

    char *identifiers_str[] = {
        "do",
        "done",
        "case",
        "esac",
        "function",
        "select",
        "until",
        "if",
        "elif",
        "fi",
        "then",
        "while",
        "else",
        "for",
        "in",
        "time",
        "return"
    };

    TokenType identifiers_name[] = {
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
    };

    for (int i = 0; i < KEYWORDS_LEN; i++) {
        char *raw_str = identifiers_str[i];
        ht_set(identifiers, raw_str, strlen(raw_str), &identifiers_name[i], sizeof(TokenType), NULL);
    }
}

static void destroy_identifiers(void)
{
    ht_free(identifiers);
}

static struct lex_t *lex_malloc(void)
{
    Lex *lx = malloc(sizeof(Lex));
    lx->size = 0;
    lx->capacity = 32;
    lx->tokens = malloc(32 * sizeof(TokenType *));
    return lx;
}

void lex_free(Lex *lx)
{
    free(lx->tokens);
    free(lx);
}

Lex *tokenize(char *source)
{
    init_identifiers();
    Lex *lx = lex_malloc();

    /* main lexical analysis loop */
    char c;
    while ((c = *source++) != 0) {
        switch (c) {
        }
    }

    destroy_identifiers();

    return lx;
}

void lex_dump(struct lex_t *lx);
