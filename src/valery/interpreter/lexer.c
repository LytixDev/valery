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
//SPEC: https://pubs.opengroup.org/onlinepubs/9699919799/utilities/V3_chap02.html#tag_18_10
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <assert.h>
#ifdef DEBUG_INTERPRETER
#       include <stdio.h>
#endif

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
struct darr_t *ptokens;
bool first_word;

/* functions */

/*
 * fills the global hashtable 'identifiers' with the supported identifiers
 */
static void init_identifiers(void)
{
    if (identifiers != NULL)
        return;

    identifiers = ht_alloc(32);

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
        ht_sset_alloc_T(identifiers, raw_str, &identifiers_name[i], enum tokentype_t);
    }
}

static inline void destroy_identifiers(void)
{
    ht_free(identifiers);
}

#define expansion_tokens_alloc(p, s) expansion_alloc(ET_TOKENLIST, p, s)
struct expansion_t *expansion_alloc(enum expansion_type type, void *p, size_t size)
{
    struct expansion_t *expansion = vmalloc(sizeof(struct expansion_t));
    expansion->type = type;

    if (type == ET_TOKENLIST) {
        expansion->tokens = p;
    } else {
        /* make copy of string from source code and add null byte */
        char *cpy = vmalloc(sizeof(char) * (size + 1));
        strncpy(cpy, (char *)p, size);
        cpy[size] = 0;
        expansion->str = cpy;
    }

    return expansion;
}

static struct token_t *token_alloc(enum tokentype_t type, void *p, size_t lexeme_size,
                                    void *literal, size_t literal_size)
{
    struct token_t *token = vmalloc(sizeof(struct token_t));
    token->type = type;

    /* expansions store a dynamic array of expansions, not the token lexeme */
    if (type == T_WORD || type == T_STRING) {
        token->expansions = p;
        return token;
    }

    if (p != NULL) {
        token->lexeme = vmalloc(lexeme_size);
        strncpy(token->lexeme, p, lexeme_size);
        token->lexeme[lexeme_size - 1] = 0;
    }

    if (literal != NULL) {
        token->literal = vmalloc(literal_size);
        memcpy(token->literal, literal, literal_size);
    }

    return token;
}

#define add_expansion(t, p) add_token(t, p, 0, NULL, 0)
static void add_token(enum tokentype_t type, void *p, size_t lexeme_size, void *literal,
                      size_t literal_size)
{
    darr_append(ptokens, token_alloc(type, p, lexeme_size, literal, literal_size));
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
static bool is_special_char(char c)
{
    if (first_word) {
        switch (c) {
            case '=':
                return true;
        }
    }

    /* 2.2, without $ (handled elsewhere) */
    switch (c) {
        case '|':
        case '&':
        case ';':
        case '<':
        case '>':
        case '(':
        case ')':
        case '`':
        case '\\':
        case '"':
        case '\'':
        case ' ':
        case '\t':
        case '\n':
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

static void single_qoute(void)
{
    /* 2.2.2 */
    //TODO: this is rather ugly
    char c;
    char *literal_start = source_cpy;           // not -1 because we ignore the first qoute 
    while ((c = *source_cpy) != 0) {
        source_cpy++;
        if (c == '"')
            break;
    }

    if (*source_cpy == 0)
        valery_exit_parse_error("string not terminated");

    size_t literal_size = source_cpy - literal_start - 1;
    add_token(T_STRING, NULL, 0, literal_start, literal_size);
}

static bool expansion_finished(enum expansion_type et)
{
    if (first_word && et == ET_LITERAL && *source_cpy == '=') {
        first_word = false;
        return true;
    }

    if (et == ET_EXPAND && is_special_char(*source_cpy))
        return true;

    if (*source_cpy == '$')
        return true;

    return false;
}

static enum expansion_type expansion_determine_type(void)
{
    if (*source_cpy == '$') {
        if (*source_cpy + 1 == '(') {
            /* consume the paren */
            source_cpy++;
            return ET_TOKENLIST;
        } else {
            return ET_EXPAND;
        }
    }
    return ET_LITERAL;
}

static void double_qoute(void)
{
    struct darr_t *expansions = darr_malloc();
    enum expansion_type et;
    char *expansion_start, *expansion_end;

    expansion_start = source_cpy - 1;    // -1 because scan_token() incremented source_cpy
    et = expansion_determine_type();

    while (*source_cpy != '"') {
        if (expansion_finished(et)) {
            /* store previous expansion */
            expansion_end = source_cpy - 1;
            assert(expansion_start <= expansion_end);
            darr_append(expansions, expansion_alloc(et, expansion_start,
                                                    expansion_end - expansion_start + 1));

            expansion_start = ++source_cpy;
            et = expansion_determine_type();
        } else {
            source_cpy++;
        }
    }

    /* append last expansion */
    expansion_end = source_cpy - 1;     // don't want to keep the final '"'
    assert(expansion_start <= expansion_end);
    darr_append(expansions, expansion_alloc(et, expansion_start,
                                            expansion_end - expansion_start + 1));
    add_expansion(T_STRING, expansions);
}

static void word(void)
{
    struct darr_t *expansions = darr_malloc();
    enum expansion_type et;
    char *expansion_start, *expansion_end;

    expansion_start = source_cpy - 1;    // -1 because scan_token() incremented source_cpy
    et = expansion_determine_type();

    while (*source_cpy != ' ') {
        if (expansion_finished(et)) {
            /* store previous expansion */
            expansion_end = source_cpy - 1;     // don't want to keep the '$'
            assert(expansion_start <= expansion_end);
            darr_append(expansions, expansion_alloc(et, expansion_start,
                                                    expansion_end - expansion_start + 1));

            expansion_start = ++source_cpy;
            et = expansion_determine_type();
        } else {
            source_cpy++;
        }
    }

    /* append last expansion */
    expansion_end = source_cpy - 1;     // don't want to keep the final '"'
    assert(expansion_start <= expansion_end);
    darr_append(expansions, expansion_alloc(et, expansion_start,
                                            expansion_end - expansion_start + 1));
    add_expansion(T_WORD, expansions);
}

/* 
 * scans the source code until a non-ambigious token is determined 
 * if the determined token shall set the first_word variable to false,
 * it breaks out of the switch statement, if not, the function simply returns
 * after determining the token.
 */
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
            first_word = true;
            return;
        case '*':
            add_token_simple(T_STAR);
            break;
        //case '$':
        //    add_token_simple(T_DOLLAR);
        //    break;

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
            //if (*source_cpy == 0) {
            //    /* '!' was last char in source file */
            //    add_token_simple(T_BANG);
            //    break;
            //}
            if (*source_cpy == '!') {
                source_cpy++;
                add_token_simple(T_BANG_BANG);
                break;
            } else {
                word();
                break;
            }

            //else if (next == '=') {
            //    source_cpy++;
            //    add_token_simple(T_BANG_EQUAL);
            //    break;
            //}

            /* no other match found */
            //add_token_simple(T_BANG);
            break;


        /* ignore comments */
        case '#':
            while (*source_cpy != 0 && *source_cpy != '\n')
                source_cpy++;

            source_cpy++;       // go past newline
            first_word = true;
            return;


        /* ignore all whitespace */
        case ' ':
        case '\r':
        case '\t':
            break;


        case '\n':
            add_token_simple(T_NEWLINE);
            first_word = true;
            return;


        /* string literal */
        case '\'':
            single_qoute();

        case '"':
            single_qoute();
            //double_qoute();
            break;

        
        default:
            word();
    }

    if (first_word)
        first_word = false;
}

struct darr_t *tokenize(char *source)
{
    ptokens = darr_malloc();
    source_cpy = source;                // global pointer into the source code for simplicity 
    first_word = true;
    init_identifiers();

    /* main lexical analysis loop */
    char c;
    while ((c = *source_cpy) != 0)
        scan_token();                   // this function increments the source_cpy as needed

    /* add sentinel token */
    add_token(T_EOF, NULL, 0, NULL, 0);

    //destroy_identifiers();
    return ptokens;
}

void tokenlist_print(struct darr_t *tokens)
{
#ifdef DEBUG_INTERPRETER
    printf("--- lex dump ---\n");
    struct token_t *token;
    size_t upper = darr_get_size(tokens);
    for (size_t i = 0; i < upper; i++) {
        token = darr_get(tokens, i);
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
