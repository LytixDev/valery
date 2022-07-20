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
#define TOTAL_OPERANDS 5


/* types */
/* see definition of *operands[] in lexer.c for string representation of the operands */
typedef enum operands_t {
    O_NONE,  /* special case: string is not an operand */
    O_PIPE,
    O_OR,
    O_AND,
    O_RE,
    O_APP
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
struct token_t *token_t_malloc();

void token_t_free(struct token_t *t);

void token_t_resize(struct token_t *t, size_t new_size);

struct tokenized_str_t *tokenized_str_t_malloc();

void tokenized_str_t_free(struct tokenized_str_t *ts);

void tokenized_str_t_resize(struct tokenized_str_t *ts, size_t new_size);

enum operands_t get_token_operand(char *token);

/*
 * adds the char parameter to the end of the str.
 * calls token_t_resize() if str is not large enough.
 */
void token_t_append_char(struct token_t *t, char c);

/*
 * calls token_t_append_char() using the endmost token.
 */
void tokenized_str_t_append_char(struct tokenized_str_t *ts, char c);

/* just for debugging purpsos */
void tokenized_str_t_print(struct tokenized_str_t *ts);

// TODO: this is ugly
bool bool_in_list(bool *list, size_t len, bool item);

// TODO: this is ugly
int occurence_in_list(bool *list, size_t len, bool item);

// TODO: this is ugly
bool possible_delims(char c, size_t pos, bool pd[TOTAL_OPERANDS]);

// TODO: temporary solution
void trim_spaces(struct tokenized_str_t *ts);

void tokenize(struct tokenized_str_t *ts, char *buffer);

void tokenized_str_t_clear(struct tokenized_str_t *ts);

#endif
