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
#include "lib/hashtable.h"


static size_t start = 0;
static size_t cur = 0;
static size_t end;
static size_t line = 1;
static char *source_cpy = NULL;

static struct ht_t *identifiers = NULL;

#ifdef DEBUG
char *ttype_str[] =  {
    /* single-character tokens. */
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
    "T_AN",
    "T_AN_AN",
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
#endif /* DEBUG */


void init_identifiers()
{
    if (identifiers != NULL)
        return;

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

    char *key;
    ttype_t val;
    int len = 17;
    int keyword_start = 34;

    identifiers = ht_malloc(32);

    for (int i = 0; i < len; i++) {
        key = identifiers_str[i];
        val = keyword_start + i;
        ht_set(identifiers, key, strlen(key) + 1, &val, sizeof(enum ttype_t), NULL);
    }
}

void destroy_identifiers()
{
    ht_free(identifiers);
}

static void syntax_error(char c, char *msg)
{
    fprintf(stderr, "syntax error in line %zu at char '%c'.\n", line, c);
    if (msg != NULL)
        fprintf(stderr, "msg: %s\n", msg); 
    exit(1);
}

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

static inline bool eof(int n)
{
    return source_cpy[cur + n] == 0;
}

static struct token_t *token_malloc(enum ttype_t type, char *lexeme, size_t lexeme_size,
                                    void *literal, size_t literal_size)
{
    struct token_t *t = malloc(sizeof(token_t));
    t->type = type;
    t->line = line;

    if (lexeme != NULL) {
        t->lexeme = malloc(lexeme_size + 1);
        strncpy(t->lexeme, lexeme, lexeme_size);
    } else {
        t->lexeme = NULL;
    }

    t->literal_size = literal_size;
    if (literal != NULL) {
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

static void add_token_full(struct lex_t *lx, enum ttype_t type, char *lexeme, size_t lexeme_size,
                           void *literal, size_t literal_size)
{
    if (lx->size >= lx->capacity)
        lex_increase(lx, lx->capacity * 2);

    lx->tokens[lx->size++] = token_malloc(type, lexeme, lexeme_size, literal, literal_size);
}

static void add_token(struct lex_t *lx, enum ttype_t type)
{
    add_token_full(lx, type, source_cpy + start, cur - start, NULL, 0);
}


struct lex_t *lex_malloc(void)
{
    struct lex_t *lx = malloc(sizeof(struct lex_t));
    lx->size = 0;
    lx->capacity = 32;
    lx->tokens = malloc(32 * sizeof(struct token_t *));
    return lx;
}

static inline char peek(int n)
{
    if (cur + n >= end)
        return '\0';
    return source_cpy[cur + n];
}

static bool match(char expected)
{
    if (eof(0))
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
    while ((c = peek(0)) != '"' && !eof(0)) {
        if (c == '\n') line++;
        cur++;
    }

    if (eof(0))
        syntax_error(c, "string not terminated");

    /* close the string */
    cur++;

    /* trim string literal representation from quotes */
    void *string_literal = source_cpy + start + 1;
    size_t literal_size = cur - start - 2;

    add_token_full(lx, T_STRING, source_cpy + start, cur - start, string_literal, literal_size);
}

static void number(struct lex_t *lx)
{
    while (is_digit(peek(0)))
        cur++;

    ///* look for '.' determining if there is a fractional part*/
    //if (peek(0) == '.' && is_digit(peek(1))) {
    //    /* consume the '.' */
    //    cur++;

    //    while (is_digit(peek(0)))
    //        cur++;
    //}
 
    //TODO: use substring instead, and check for error
    //char sub[cur - start + 1];
    int32_t literal;
    char *p;

    literal = strtol(source_cpy + start, &p, 10);

    add_token_full(lx, T_NUMBER, source_cpy + start, cur - start, &literal, sizeof(literal));
}

static void identifier(struct lex_t *lx)
{
    while (is_alpha_numeric(peek(0)))
        cur++;

    size_t len = cur - start;
    char keyword[len + 1];
    strncpy(keyword, source_cpy + start, len + 1);
    keyword[len + 1] = 0;


    /* if not a reserved keyword, it is a user-defined identifier */
    enum ttype_t *type = ht_get(identifiers, keyword, strlen(keyword) +1);
    if (type == NULL)
        add_token(lx, T_IDENTIFIER);
    else
        add_token(lx, *type);
}

void lex_free(struct lex_t *lx)
{
    for (size_t i = 0; i < lx->size; i++)
        token_free(lx->tokens[i]);

    free(lx->tokens);
    free(lx);
}

#ifdef DEBUG
void lex_dump(struct lex_t *lx)
{
    struct token_t *t;
    printf("lex dump:\n");

    for (size_t i = 0; i < lx->size; i++) {
        t = lx->tokens[i];
        printf("%zu\n\ttype: %s\n\tlexeme: '%s'\n", i, (char *)ttype_str[t->type], t->lexeme);

        if (t->literal != NULL) {
            /* print hex representation of literal data */
            printf("\thex: ");
            for (size_t j = 0; j < t->literal_size; j++)
                printf("%02x ", ((uint8_t *)t->literal)[j]);

            putchar('\n');

            /* print string representation of literal data */
            if (t->type == T_NUMBER)
                printf("\tliteral: %d\n", *(int32_t *)t->literal);
            else
                printf("\tliteral: %s\n", (char *)t->literal);
        }

        putchar('\n');
    }

}
#endif

void tokenize(struct lex_t *lx, char *source)
{
    char c;
    source_cpy = source;
    end = strlen(source);

    while (source_cpy[cur] != 0) {
        start = cur;
        scan_token(lx);
    }

    /* add sentinel token */
    add_token_full(lx, T_EOF, NULL, 0, NULL, 0);
}

void scan_token(struct lex_t *lx)
{
    char c = source_cpy[cur++];

    switch (c) {
        /* single character lexems */
        case '(':
            add_token(lx, T_LPAREN);
            break;
        case ')':
            add_token(lx, T_RPAREN);
            break;
        case '{':
            add_token(lx, T_LBRACE);
            break;
        case '}':
            add_token(lx, T_RBRACE);
            break;
        case ';':
            add_token(lx, T_SEMICOLON);
            break;
        case ':':
            add_token(lx, T_COLON);
            break;
        case '*':
            add_token(lx, T_STAR);
            break;
        case '+':
            add_token(lx, T_PLUS);
            break;
        case '-':
            add_token(lx, T_MINUS);
            break;


        /* two character lexems */
        case '$':
            add_token(lx, match('(') ? T_DOLLAR_LPAREN : T_DOLLAR);
            break;
        case '&':
            add_token(lx, match('&') ? T_AN_AN : T_AN);
            break;
        case '!':
            if (eof(0)) {
                syntax_error(c, "end of file");
                break;
            }

            char next = peek(0);

            if (next == '!') {
                cur++;
                add_token(lx, T_BANG_BANG);
                break;
            } else if (next == '=') {
                cur++;
                add_token(lx, T_BANG_EQUAL);
                break;
            }

            /* no match found */
            add_token(lx, T_BANG);
            break;
        case '=':
            add_token(lx, match('=') ? T_EQUAL_EQUAL : T_EQUAL);
            break;
        case '.':
            add_token(lx, match('.') ? T_DOT_DOT : T_DOT);
            break;



        /* ignore comments */
        case '#':
            while (peek(0) != '\n' && !eof(0))
                cur++;
            break;


        /* ignore all whitespace */
        case ' ':
            break;
        case '\r':
            break;
        case '\t':
            break;


        /* increment line on new line */
        case '\n':
            line++;
            break;


        /* string literal */
        case '"':
            string(lx);
            break;

        default:
            if (is_digit(c)) {
                number(lx);
            } else if (is_alpha(c)) {
                identifier(lx);
            } else {
                syntax_error(c, NULL);
            }
            break;

    }

}

