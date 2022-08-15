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

#ifndef VALERY_LEXER_H
#define VALERY_LEXER_H

#include <stddef.h>
#include <stdbool.h>

#include "../env.h"

#define DEFAULT_TOKEN_SIZE 128
#define STARTING_TOKENS 8
#define TOTAL_OPERANDS 13

#define ASCII_A 0x41
#define ASCII_Z 0x5a
#define ASCII_0 0x30
#define ASCII_9 0x39
#define ASCII_UNDERSCORE 0x5f


/* types */
typedef enum parse_flags {
    //PF_SKIP     = 1 << 0,
    PF_QUOTE    = 1 << 1,
    PF_ESCAPE   = 1 << 2,
    PF_DOTDOT   = 1 << 3
} parse_flags;

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
    char *str_start;          /* points to the start of the string */
    enum operands_t type;     /* the semantic intepretation of the token */
    size_t str_len;           /* the current len of the token string without counting terminating null byte */
    size_t str_capacity;      /* the allocated capcity of the token string */
} token_t;


typedef struct source_t {
    struct token_t **tokens;  /* list of the tokens */
    size_t size;              /* total tokens occupied / in use */
    size_t capacity;          /* total tokens allocated */
} source_t;


/* extern variable definitions */
extern const char *operands_str[TOTAL_OPERANDS];
extern const operands_t operands[TOTAL_OPERANDS];


/* functions */
/* returns a pointer to a malloced token_t object with DEFAULT_TOKEN_SIZE size str */
struct token_t *token_t_malloc(void);

void token_t_free(struct token_t *t);

void token_t_resize(struct token_t *t, size_t new_capacity);

/* returns a pointer to a malloced source_t object with STARTING_TOKENS amount of token_t */
struct source_t *tokenized_str_t_malloc(void);

void tokenized_str_t_free(struct source_t *ts);

void tokenized_str_t_resize(struct source_t *ts, size_t new_capacity);

/*
 * 'cleans' the object and makes it act as though it has just been malloced.
 * does not actually wipe any token_t->str, but if sentinel null byte is
 * correctly set it does not matter.
 */
void tokenized_str_t_clear(struct source_t *ts);

void token_t_append_str(struct token_t *t, char *str);

/*
 * adds the char argument to the end of the str.
 * calls token_t_resize() if str is not large enough.
 */
void token_t_append_char(struct token_t *t, char c);

/* replaces the last char in the char array with the sentinel null byte */
void token_t_pop_char(struct token_t *t);

/*
 * increments size and returns a pointer to the next token_t
 * calls tokenized_str_t_resize() if necessary.
 */
struct token_t *tokenized_str_t_next(struct source_t *ts);

/* just for debugging */
void token_t_print(struct token_t *t);

/* just for debugging */
void tokenized_str_t_print(struct source_t *ts);

/*
 * goes through each operand and checks if the char at the 'pos' input argument matches 
 * the char 'c' argument. If they don't match, the candidate for that operand
 * is set to false if it was true and decrements total_candidates.
 *
 * returns the value of total_candidates.
 */
int update_candidates(char c, size_t pos, bool candidates[TOTAL_OPERANDS], int *total_candidates);

/*
 * returns the first operand set to true in the list
 * returns O_NONE if no operand is set to true
 */
operands_t which_operand(const bool candidates[TOTAL_OPERANDS]);

/* prints a nice syntax error to stderr */
void print_syntax_error(const char *buf_start, const char *buf_err, char *msg);

/*
 * splits the input buffer into tokens based using operands in operands_str as delimiters.
 */
int tokenize(struct source_t *ts, struct env_vars_t *env_vars, char *buffer);

/*
 * checks if the given char c argument is a special char, and deals with it accordingly.
 * returns true if char is special and was dealt with, else false.
 */
bool special_char(struct env_vars_t *env_vars, struct token_t *t, char c, char **buffer, unsigned int *p_flags);

int str_to_argv(char *str, char **argv, int *argv_cap);


#endif /* VALERY_LEXER_H */
