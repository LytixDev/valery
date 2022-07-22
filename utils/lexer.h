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
#include <stddef.h>
#include <stdbool.h>


#ifndef LEXER
#define LEXER

#define DEFAULT_TOKEN_SIZE 128
#define STARTING_TOKENS 8
#define TOTAL_OPERANDS 13


/* types */
/* see definition of *operands[] in lexer.c for string representation of the operands */
typedef enum operands_t {
    O_NONE = -1,  /* special case: string is not an operand */
    O_PIPE,
    O_OR,
    O_AMP,
    O_AND,
    O_OUTP,
    O_OUPP,
    O_INP,
    O_INPP,
    O_SEMI,
    O_INPAR,
    O_OUTPAR,
    O_INBRA,
    O_OUTBRA
} operands_t;


typedef struct token_t {
    char *str;                /* the actual token string */
    enum operands_t type;     /* the semantic intepretation of the token */
    size_t str_len;           /* the current len of the token string without counting terminating null byte */
    size_t str_allocated;     /* the allocated size of the token string */
} token_t;


typedef struct tokenized_str_t {
    struct token_t **tokens;  /* list of the tokens */
    size_t total_tokens;      /* actual total tokens in the token list */
    size_t tokens_allocated;  /* totoal allocated tokens */
} tokenized_str_t;


/* extern variable definitions */
extern const char *operands_str[TOTAL_OPERANDS];
extern const operands_t operands[TOTAL_OPERANDS];


/* functions */
/* returns a pointer to a malloced token_t object with DEFAULT_TOKEN_SIZE size str */
struct token_t *token_t_malloc();

void token_t_free(struct token_t *t);

void token_t_resize(struct token_t *t, size_t new_size);

/* returns a pointer to a malloced tokenized_str_t object with STARTING_TOKENS amount of token_t */
struct tokenized_str_t *tokenized_str_t_malloc();

void tokenized_str_t_free(struct tokenized_str_t *ts);

void tokenized_str_t_resize(struct tokenized_str_t *ts, size_t new_size);

/*
 * 'cleans' the object and makes it act as though it has just been malloced.
 * does not actually wipe any token_t->str, but if sentinel null byte is
 * correctly set it does not matter.
 */
void tokenized_str_t_clear(struct tokenized_str_t *ts);

/*
 * adds the char parameter to the end of the str.
 * calls token_t_resize() if str is not large enough.
 */
void token_t_append_char(struct token_t *t, char c);

/* replaces the last char in the char array with the sentinel null byte */
void token_t_pop_char(struct token_t *t);

/* calls token_t_append_char() using the endmost token */
void tokenized_str_t_append_char(struct tokenized_str_t *ts, char c);

/* 
 * increments total_tokens and returns a pointer to the next token_t
 * calls tokenized_str_t_resize() if necessary.
 */
struct token_t *tokenized_str_t_next(struct tokenized_str_t *ts);

/* just for debugging */
void tokenized_str_t_print(struct tokenized_str_t *ts);

/*
 * goes through each operand and checks if the char at the 'pos' input parameter matches 
 * the char 'c' input parameter. If they don't match, the candidate for that operand
 * is set to false if it was true and decrements total_candidates.
 *
 * returns the value of total_candidates.
 */
int update_candidates(char c, size_t pos, bool candidates[TOTAL_OPERANDS], int *total_candidates);

/*
 * returns the first operand set to true in the list
 * returns O_NONE if no operand is set to true
 */
operands_t which_operand(bool candidates[TOTAL_OPERANDS]);

/* prints a nice syntax error to stderr */
void print_syntax_error(const char *buf_start, char *buf_err);

// TODO: add short explanation
int tokenize(struct tokenized_str_t *ts, char *buffer);

// TODO: temporary solution
void trim_spaces(struct tokenized_str_t *ts);


#endif
