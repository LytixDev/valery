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
#include <string.h>     // strlen, strncpy, memcpy
#include <stdio.h>      // debug: printf
#include <stdbool.h>    // bool type

#include "lex.h"
#define NICC_HT_IMPLEMENTATION
#include "../nicc/nicc.h"  // hashtable implementation
#include "../vcommon.h"


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

const char *tokentype_str[] = {
    /* keywords */
    "T_DO",
    "T_DONE",
    "T_CASE",
    "T_ESAC",
    "T_FUNCTION",
    "T_SELECT",
    "T_UNTIL",
    "T_IF",
    "T_ELIF",
    "T_FI",
    "T_THEN",
    "T_WHILE",
    "T_ELSE",
    "T_FOR",
    "T_IN",
    "T_TIME",
    "T_RETURN",

    /* single-character tokens */
    "T_LPAREN",
    "T_RPAREN",
    "T_LBRACE",
    "T_RBRACE",
    "T_COMMA",
    "T_MINUS",
    "T_PLUS",
    "T_COLON",
    "T_SEMICOLON",
    "T_SLASH",
    "T_STAR",

    /* one or two character tokens */
    "T_DOLLAR",
    "T_DOLLAR_LPAREN",
    "T_ANP",
    "T_ANP_ANP",
    "T_BANG",
    "T_BANG_BANG",
    "T_BANG_EQUAL",
    "T_EQUAL",
    "T_EQUAL_EQUAL",
    "T_GREATER",
    "T_GREATER_EQUAL",
    "T_LESS",
    "T_LESS_EQUAL",
    "T_LBRACKET",
    "T_LBRACKET_LBRACKET",
    "T_RBRACKET",
    "T_RBRACKET_RBRACKET",
    "T_DOT",
    "T_DOT_DOT",

    /* literals */
    "T_IDENTIFIER",
    "T_STRING",
    "T_NUMBER",

    "T_UNKNOWN",
    "T_EOF",
};


/* types */

struct token_t {
    enum ttype_t type;
    //TODO: union mayhaps/perchance?
    char *lexeme;
    void *literal;
    size_t literal_size;
    //size_t line;
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
char *source_cpy;
Lex *lx;

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
        ht_set(identifiers, raw_str, strlen(raw_str) + 1, &identifiers_name[i], sizeof(TokenType),
               NULL);
    }
}

static void destroy_identifiers(void)
{
    ht_free(identifiers);
}

//TODO use global properly
static struct lex_t *lex_malloc(void)
{
    Lex *lx = malloc(sizeof(Lex));
    lx->size = 0;
    lx->capacity = 32;
    lx->tokens = malloc(32 * sizeof(TokenType *));
    return lx;
}

static void lex_increase(void)
{
    size_t new_capacity = lx->capacity * 2;
    lx->tokens = realloc(lx->tokens, new_capacity * sizeof(struct Token *));
    lx->capacity = new_capacity;
}

static Token *token_malloc(TokenType type, char *lexeme, size_t lexeme_size, void *literal,
                           size_t literal_size)
{
    Token *token = malloc(sizeof(Token));
    token->type = type;

    if (lexeme != NULL) {
        token->lexeme = malloc(lexeme_size + 1);
        strncpy(token->lexeme, lexeme, lexeme_size);
        token->lexeme[lexeme_size] = 0;
    }

    if (literal != NULL) {
        token->literal = malloc(literal_size);
        memcpy(token->literal, literal, literal_size);
    }

    return token;
}

static void add_token(TokenType type, char *lexeme, size_t lexeme_size, void *literal,
                      size_t literal_size)
{
    Token *token = token_malloc(type, lexeme, lexeme_size, literal, literal_size);
    if (lx->size >= lx->capacity)
        lex_increase();

    lx->tokens[lx->size++] = token_malloc(type, lexeme, lexeme_size, literal, literal_size);
}

static inline void add_token_simple(TokenType type)
{
    add_token(type, NULL, 0, NULL, 0);
}

/* helper functions */
static inline bool is_digit(char c)
{
    return c >= '0' && c <= '9';
}

static inline bool is_alpha(char c)
{
    return (c >= 'a' && c <= 'z') ||
           (c >= 'A' && c <= 'Z') ||
            c == '_';
}

static bool is_alpha_numeric(char c)
{
    return is_alpha(c) || is_digit(c);
}

/* returns true if current char of source_cpy == expected */
static bool match(char expected)
{
    //TODO: check if out of bounds?
    if (*source_cpy == expected) {
        source_cpy++;
        return true;
    }

    return false;
}

static void number_literal()
{
    char *literal_start = source_cpy - 1;       // -1 because scan_token() incremented source_cpy
    while (is_digit(*source_cpy))
        source_cpy++;

    //TODO look for '.' determining if there is a fractional part
    //TODO: use substring instead, and check for error

    char *literal_end = source_cpy - 1;         // -1 because we have gone one past the last digit
    int64_t literal = strtol(literal_start, &literal_end, 10);
    add_token(T_NUMBER, NULL, 0, &literal, sizeof(literal));
}

static void string_literal()
{
    //TODO: this is rather ugly
    char c;
    char *literal_start = source_cpy;           // not -1 because we ignore the first qoute 
    while ((c = *source_cpy) != 0) {
        if (c == '"')
            break;
        source_cpy++;
    }

    if (*source_cpy == 0)
        valery_exit_parse_error("string not terminated");

    size_t literal_size = source_cpy - literal_start;
    /* close the string by moving past the last qoute */
    source_cpy++;
    add_token(T_STRING, NULL, 0, literal_start, literal_size);
}

static void identifier()
{
    char *identifier_start = source_cpy - 1;    // -1 because scan_token() incremented source_cpy
    while (is_alpha_numeric(*source_cpy))
        source_cpy++;

    size_t len = source_cpy - identifier_start;
    char identifier[len];
    strncpy(identifier, identifier_start, len);
    identifier[len] = 0;

    /* if not a reserved keyword, it is a user-defined identifier */
    TokenType *type = ht_get(identifiers, identifier, len + 1);
    add_token(type == NULL ? T_IDENTIFIER : *type, identifier, len + 1, NULL, 0);
}


/* scans the source code until a non-ambigious token is determined */
static void scan_token()
{
    char c = *source_cpy++;
    switch (c) {
        /* single character lexems */
        case '(':
            add_token_simple(T_LPAREN);
            break;
        case ')':
            add_token_simple(T_RPAREN);
            break;
        case '{':
            add_token_simple(T_LBRACE);
            break;
        case '}':
            add_token_simple(T_RBRACE);
            break;
        case ';':
            add_token_simple(T_SEMICOLON);
            break;
        case ':':
            add_token_simple(T_COLON);
            break;
        case '*':
            add_token_simple(T_STAR);
            break;
        case '+':
            add_token_simple(T_PLUS);
            break;
        case '-':
            add_token_simple(T_MINUS);
            break;


        /* two character lexems */
        case '$':
            add_token_simple(match('(') ? T_DOLLAR_LPAREN : T_DOLLAR);
            break;
        case '&':
            add_token_simple(match('&') ? T_ANP_ANP : T_ANP);
            break;
        case '=':
            add_token_simple(match('=') ? T_EQUAL_EQUAL : T_EQUAL);
            break;
        case '.':
            add_token_simple(match('.') ? T_DOT_DOT : T_DOT);
            break;
        case '!':
            if (*source_cpy == 0) {
                /* '!' was last char in source file */
                add_token_simple(T_BANG);
                break;
            }

            char next = *source_cpy;
            if (next == '!') {
                source_cpy++;
                add_token_simple(T_BANG_BANG);
                break;
            } else if (next == '=') {
                source_cpy++;
                add_token_simple(T_BANG_EQUAL);
                break;
            }

            /* no other match found */
            add_token_simple(T_BANG);
            break;


        /* ignore comments */
        case '#':
            while (*source_cpy != 0 && *source_cpy != '\n')
                source_cpy++;
            break;


        /* ignore all whitespace and new-lines */
        case ' ':
            break;
        case '\r':
            break;
        case '\t':
            break;
        case '\n':
            break;


        /* string literal */
        case '"':
            string_literal();
            break;

        
        default:
            if (is_digit(c))
                number_literal();
            else if (is_alpha(c))
                identifier();
            else
                valery_exit_parse_error("sucks man");
            break;
    }
}

Lex *tokenize(char *source)
{
    lx = lex_malloc();          // define global Lex type
    source_cpy = source;        // global pointer into the source code for simplicity 
    init_identifiers();

    /* main lexical analysis loop */
    char c;
    while ((c = *source_cpy) != 0)
        scan_token();           // this function increments the source_cpy as needed

    /* add sentinel token */
    add_token(T_EOF, NULL, 0, NULL, 0);
    destroy_identifiers();
    return lx;
}

void lex_dump(Lex *lx)
{
    printf("--- lex dump ---\n");
    Token *token;
    for (size_t i = 0; i < lx->size; i++) {
        token = lx->tokens[i];
        printf("type: %-16s|", tokentype_str[token->type]);
        //printf("%s", token->lexeme == NULL ? "(null)" : token->lexeme);

        if (token->literal != NULL) {
            if (token->type == T_NUMBER)
                printf(" literal: '%ld'", *(int64_t *)token->literal);
            else if (token->type == T_STRING)
                printf(" literal: '%s'", (char *)token->literal);
        }

        if (token->lexeme != NULL)
            printf(" lexeme: '%s'", token->lexeme);

        putchar('\n');
    }
}
