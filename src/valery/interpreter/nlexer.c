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

static inline bool eof(char c)
{
    return c == 0;
}

static struct token_t *token_malloc(enum ttype_t type, size_t line, char *lexeme,
                                    size_t lexeme_size, void *literal, size_t literal_size)
{
    struct token_t *t = malloc(sizeof(token_t));
    t->type = type;
    t->line = line;

    t->lexeme = malloc(lexeme_size);
    strncpy(t->lexeme, lexeme, lexeme_size);

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

static void lex_add_token(struct lex_t *lx, enum ttype_t type, size_t line, char *lexeme,
                          size_t lexeme_size, void *literal, size_t literal_size)
{
    if (lx->size >= lx->capacity)
        lex_increase(lx, lx->capacity * 2);

    lx->tokens[lx->size++] = token_malloc(type, line, lexeme, lexeme_size, literal,
                                          literal_size);
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
        printf("%zu\n\ttype: %d\n\tlexeme: %s\n\tliteral: ", i, t->type, t->lexeme);

        for (size_t j = 0; j < t->literal_size; j++)
            printf("%02x ", ((uint8_t *)t->literal)[j]);

        printf("\n\n");
    }

}

size_t start = 0;
size_t cur = 0;
size_t line = 1;

void tokenize(struct lex_t *lx, char *source)
{
    char c;

    while ((c = source[cur]) != 0) {
        start = cur;
        //scan_token(lx);

        source++;
    }

    /* add sentinel token */
    lex_add_token(lx, T_EOF, line, "", 1, NULL, 0);

    lex_dump(lx);
}

void scan_token(struct lex_t *lx, char *source)
{
    char c = (*source)++;

    switch (c) {
    }

}
