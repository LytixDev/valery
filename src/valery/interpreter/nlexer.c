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
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>

#include "valery/interpreter/nlexer.h"


size_t start = 0;
size_t cur = 0;
size_t end;
size_t line = 1;
char *source_cpy = NULL;

char *ttype_str[] =  {
    /* single-character tokens. */
    "T_LPAREN",
    "T_RPAREN",
    "T_LBRACE",
    "T_RBRACE",
    "T_COMMA",
    "T_DOT",
    "T_MINUS",
    "T_PLUS",
    "T_SEMICOLON",
    "T_SLASH",
    "T_STAR",

    /* one or two character tokens */
    "T_BANG",
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

    /* literals */
    "T_IDENTIFIER",
    "T_STRING",
    "T_NUMBER",
    "T_BUILTIN",

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

    "T_UNKNOWN",
    "T_EOF",
};


static bool is_digit(char c)
{
    return c >= '0' && c <= '9';
}

static bool is_alpha(char c)
{
    return (c >= 'a' && c <= 'z') ||
           (c >= 'A' && c <= 'Z') ||
            c == '_';
}

static bool is_alpha_numeric(char c)
{
    return is_alpha(c) || is_digit(c);
}

static inline bool eof()
{
    return source_cpy[cur] == 0;
}

static inline char peek(int n)
{
    if (cur + n >= end)
        return '\0';
    return source_cpy[cur + n];
}

static bool match(char expected)
{
    if (eof())
        return false;

    if(source_cpy[cur] != expected)
        return false;

    /* only advance if cur was matched with expected */
    cur++;
    return true;
}

static void string(struct lex_t *lx)
{
    char c;
    while ((c = peek(0)) != '"' && !eof()) {
        if (c == '\n') line++;
        cur++;
    }

    if (eof()) {
        fprintf(stderr, "string not terminated\n");
        exit(1);
    }

    /* close the string */
    cur++;

    /* trim string from quotes */
    //String value = source.substring(start + 1, current - 1);
    addToken(lx, T_STRING);
}

static struct token_t *token_malloc(enum ttype_t type, void *literal, size_t literal_size)
{
    struct token_t *t = malloc(sizeof(token_t));
    t->type = type;
    t->line = line;

    if (type != T_EOF) {
        t->lexeme = malloc(cur - start + 1);
        strncpy(t->lexeme, source_cpy + start, cur - start);
    } else {
        t->lexeme = NULL;
    }

    t->literal_size = literal_size;
    if (literal_size != 0) {
        t->literal = malloc(literal_size);
        memcpy(t->literal, literal, literal_size);
    } else {
        t->literal = NULL;
    }

    return t;
}

static void token_free(struct token_t *t)
{
    free(t->lexeme);
    if (t->literal != NULL)
        free(t->literal);
    free(t);
}

static void lex_increase(struct lex_t *lx, size_t new_capacity)
{
    lx->tokens = realloc(lx->tokens, new_capacity * sizeof(struct token_t *));
    lx->capacity = new_capacity;
}

static void lex_add_token(struct lex_t *lx, enum ttype_t type, void *literal, size_t literal_size)
{
    if (lx->size >= lx->capacity)
        lex_increase(lx, lx->capacity * 2);

    lx->tokens[lx->size++] = token_malloc(type, literal, literal_size);
}

struct lex_t *lex_malloc(void)
{
    struct lex_t *lx = malloc(sizeof(struct lex_t));
    lx->size = 0;
    lx->capacity = 32;
    lx->tokens = malloc(32 * sizeof(struct token_t *));
    return lx;
}

void lex_free(struct lex_t *lx)
{
    for (size_t i = 0; i < lx->size; i++)
        token_free(lx->tokens[i]);

    free(lx->tokens);
    free(lx);
}

void lex_dump(struct lex_t *lx)
{
    struct token_t *t;
    printf("lex dump:\n");

    for (size_t i = 0; i < lx->size; i++) {
        t = lx->tokens[i];
        printf("%zu\n\ttype: %s\n\tlexeme: '%s'\n", i, (char *)ttype_str[t->type], t->lexeme);

        if (t->literal != NULL) {
            printf("\tliteral: ");
            for (size_t j = 0; j < t->literal_size; j++)
                printf("%02x ", ((uint8_t *)t->literal)[j]);

            putchar('\n');
        }

        putchar('\n');
    }

}

void addToken(struct lex_t *lx, enum ttype_t type)
{
    lex_add_token(lx, type, NULL, 0);
}

void tokenize(struct lex_t *lx, char *source)
{
    char c;
    source_cpy = source;
    // ugly
    end = strlen(source);

    while (source_cpy[cur] != 0) {
        start = cur;
        scan_token(lx);
    }

    /* add sentinel token */
    lex_add_token(lx, T_EOF, NULL, 0);

    lex_dump(lx);
}

void scan_token(struct lex_t *lx)
{
    char c = source_cpy[cur++];

    switch (c) {
        /* single character lexems */
        case '(':
            addToken(lx, T_LPAREN);
            break;
        case ')':
            addToken(lx, T_RPAREN);
            break;

        /* two character lexems */
        case '=':
            addToken(lx, match('=') ? T_EQUAL_EQUAL : T_EQUAL);
            break;

        /* ignore comments */
        case '#':
            while (peek(0) != '\n' && !eof())
                cur++;
            break;

        /* ignore all whitespace */
        case ' ':
            break;
        case '\r':
            break;
        case '\t':
            break;

        case '\n':
            line++;
            break;

        /* string literal */
        case '"':
            string(lx);
            break;

        default:
            break;

    }

}

