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
//SPEC: https://pubs.opengroup.org/onlinepubs/9699919799/utilities/V3_chap02.html#tag_18_10
#include <stdlib.h>             // free
#include <string.h>             // strlen, strncpy, memcpy
#include <stdio.h>              // debug: printf
#include <stdbool.h>            // bool type

#include "valery/interpreter/lexer.h"
#include "valery/valery.h"
#ifndef NICC_IMPLEMENTATION
#       define NICC_IMPLEMENTATION
#endif
#include "lib/nicc/nicc.h"      // hashtable implementation


/* types */
#ifdef DEBUG_INTERPRETER
const char *tokentype_str[T_ENUM_COUNT] = {
    /* keywords */
    "T_IF",
    "T_THEN",
    "T_ELSE",
    "T_ELIF",
    "T_FI",
    "T_DO",
    "T_DONE",
    "T_CASE",
    "T_ESAC",
    "T_WHILE",
    "T_UNTIL",
    "T_FOR",
    "T_RETURN",
    "T_IN",

    /* single-character tokens */
    "T_LPAREN",
    "T_RPAREN",
    "T_LBRACE",
    "T_RBRACE",
    "T_SEMICOLON",
    "T_STAR",
    "T_DOLLAR",

    /* one or two character tokens */
    "T_ANP",
    "T_AND_IF",
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
    "T_PIPE",
    "T_PIPE_PIPE",

    /* symbols/identifiers */
    "T_IDENTIFIER",
    "T_WORD",
    "T_ASSIGNMENT_WORD",
    "T_NAME",
    "T_NEWLINE",
    "IO_NUMBER",
    "T_STRING",
    "T_NUMBER",

    "T_UNKNOWN",
    "T_EOF",
};
#endif /* DEBUG_INTERPRETER */


/* globals */
static struct ht_t *identifiers = NULL;
char *source_cpy;
struct tokenlist_t *tl;

/* functions */

/*
 * fills the global hashtable 'identifiers' with the supported identifiers
 */
static void init_identifiers(void)
{
    if (identifiers != NULL)
        return;

    identifiers = ht_malloc(32);

    char *identifiers_str[] = {
        "if",
        "then",
        "else",
        "elif",
        "fi",
        "do",
        "done",
        "case",
        "esac",
        "while",
        "until",
        "for",
        "return",
        "in"
    };

    enum tokentype_t identifiers_name[] = {
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
    };

    for (int i = 0; i < KEYWORDS_LEN; i++) {
        char *raw_str = identifiers_str[i];
        ht_set(identifiers, raw_str, strlen(raw_str) + 1, &identifiers_name[i], sizeof(enum tokentype_t),
               NULL);
    }
}

static inline void destroy_identifiers(void)
{
    ht_free(identifiers);
}

//TODO use global properly
static struct tokenlist_t *tokenlist_malloc(void)
{
    struct tokenlist_t *tokenlist = vmalloc(sizeof(struct tokenlist_t));
    tokenlist->pos = 0;
    tokenlist->size = 0;
    tokenlist->capacity = 32;
    tokenlist->tokens = vmalloc(32 * sizeof(enum tokentype_t *));
    return tokenlist;
}

static void tokenlist_increase(void)
{
    size_t new_capacity = tl->capacity * 2;
    tl->tokens = vrealloc(tl->tokens, new_capacity * sizeof(struct token_t *));
    tl->capacity = new_capacity;
}

static struct token_t *token_malloc(enum tokentype_t type, char *lexeme, size_t lexeme_size,
                                    void *literal, size_t literal_size)
{
    struct token_t *token = vmalloc(sizeof(struct token_t));
    token->type = type;

    if (lexeme != NULL) {
        token->lexeme = vmalloc(lexeme_size + 1);
        strncpy(token->lexeme, lexeme, lexeme_size);
        token->lexeme[lexeme_size] = 0;
    }

    if (literal != NULL) {
        token->literal = vmalloc(literal_size);
        memcpy(token->literal, literal, literal_size);
    }

    return token;
}

static void add_token(enum tokentype_t type, char *lexeme, size_t lexeme_size, void *literal,
                      size_t literal_size)
{
    if (tl->size >= tl->capacity)
        tokenlist_increase();

    tl->tokens[tl->size++] = token_malloc(type, lexeme, lexeme_size, literal, literal_size);
}

static inline void add_token_simple(enum tokentype_t type)
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

/* returns true on terminal chars for identifiers, else false */
static bool is_terminal(char c)
{
    switch (c) {
        case '|':
        case ')':
        case ']':
        case '}':
        case '(':
        case '[':
        case '{':
        case ';':
        case '&':
        case '<':
        case '>':
        case '\n':
        case ' ':
        case 0:
            return true;

        default:
            return false;
    }
}

/*
 * 3.235
 * In the shell command language, a word consisting solely of underscores, digits, and alphabetics
 * from the portable character set. The first character of a name is not a digit. 
 */
static bool is_name(char c)
{
    return is_alpha(c) || is_digit(c) || c == '_';
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

static void number_literal(void)
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

static void string_literal(void)
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

static void word(void)
{
    char *identifier_start = source_cpy - 1;    // -1 because scan_token() incremented source_cpy
    while (!is_terminal(*source_cpy))
        source_cpy++;

    size_t len = source_cpy - identifier_start;
    char identifier[len];
    strncpy(identifier, identifier_start, len);
    identifier[len] = 0;

    /*
     * 2.10.2
     * When the TOKEN is exactly a reserved word, the token identifier for that reserved word shall
     * result. Otherwise, the token WORD shall be returned. Also, if the parser is in any state
     * where only a reserved word could be the next correct token, proceed as above. 
     */
    enum tokentype_t *is_reserved = ht_get(identifiers, identifier, len + 1);
    add_token(is_reserved == NULL ? T_WORD : *is_reserved, identifier, len + 1, NULL, 0);
}

/* scans the source code until a non-ambigious token is determined */
static void scan_token(void)
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
        case '*':
            add_token_simple(T_STAR);
            break;
        case '$':
            add_token_simple(T_DOLLAR);
            break;

        /* two character lexems */
        case '&':
            add_token_simple(match('&') ? T_AND_IF : T_ANP);
            break;
        case '=':
            add_token_simple(match('=') ? T_EQUAL_EQUAL : T_EQUAL);
            break;
        case '.':
            add_token_simple(match('.') ? T_DOT_DOT : T_DOT);
            break;
        case '|':
            add_token_simple(match('|') ? T_PIPE_PIPE : T_PIPE);
            break;
        case '>':
            add_token_simple(match('=') ? T_GREATER_EQUAL : T_GREATER);
            break;
        case '<':
            add_token_simple(match('=') ? T_LESS_EQUAL : T_LESS);
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

            source_cpy++;       // go past newline
            break;


        /* ignore all whitespace */
        case ' ':
            break;
        case '\r':
            break;
        case '\t':
            break;


        case '\n':
            add_token_simple(T_NEWLINE);
            break;


        /* string literal */
        case '"':
            string_literal();
            break;

        
        default:
            word();
    }
}

struct tokenlist_t *tokenize(char *source)
{
    tl = tokenlist_malloc();            // define global struct tokenlist_t type
    source_cpy = source;                // global pointer into the source code for simplicity 
    init_identifiers();

    /* main lexical analysis loop */
    char c;
    while ((c = *source_cpy) != 0)
        scan_token();                   // this function increments the source_cpy as needed

    /* add sentinel token */
    add_token(T_EOF, NULL, 0, NULL, 0);
    //destroy_identifiers();
    return tl;
}

void tokenlist_free(struct tokenlist_t *tokenlist)
{
    for (size_t i = 0; i < tokenlist->size; i++)
        free(tokenlist->tokens[i]);

    free(tokenlist->tokens);
    free(tokenlist);
}

void tokenlist_dump(struct tokenlist_t *tokenlist)
{
#ifdef DEBUG_INTERPRETER
    printf("--- lex dump ---\n");
    struct token_t *token;
    for (size_t i = 0; i < tokenlist->size; i++) {
        token = tokenlist->tokens[i];
        printf("type: %-16s|", tokentype_str[token->type]);

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
#endif /* DEBUG_INTERPRETER */
}
