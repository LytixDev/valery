/*
 *  Tokenize input buffer for further parsing.
 *  (Is this too much voodoo for our purposes?)
 *
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

#include "valery/lexer.h"
#include "valery/env.h"
#include "valery.h"

/*
 * enum representation is found in lexer.h.
 * comment dextral to the str is the integral constant.
 * since first value in enum is 'O_NONE' = 0, use 'O_<x>' - 1 to get str
 * representation given an integral constant.
 */
const char *operands_str[] = {
    "|",  /* O_PIPE    */
    "||", /* O_OR      */
    "&",  /* O_AMP     */
    "&&", /* O_AND     */
    ">",  /* O_OUTP    */
    ">>", /* O_OUPP    */
    "<",  /* O_INP     */
    "<<", /* O_INPP    */
    ";",  /* O_SEMI    */
    "(",  /* O_INPAR   */
    ")",  /* O_OUTPAR  */
    "[",  /* O_INBRA   */
    "]"   /* O_OUTBRA  */
};

const operands_t operands[] = {
    O_PIPE, /* 0       */
    O_OR,   /* 1       */
    O_AMP,  /* 2 etc.. */
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
};


struct token_t *token_t_malloc()
{
    struct token_t *t = (struct token_t *) malloc(sizeof(struct token_t));
    t->str = (char *) malloc(DEFAULT_TOKEN_SIZE * sizeof(char));
    t->str_start = t->str;
    t->type = O_NONE;  /* set default type to none i.e, token is a program name */
    t->str_len = 0;
    t->str_capacity = DEFAULT_TOKEN_SIZE;

    return t;
}

void token_t_free(struct token_t *t)
{
    free(t->str);
    free(t);
}

void token_t_resize(struct token_t *t, size_t new_capacity)
{
    //TODO: only works when INCREASING size
    t->str = (char *) realloc(t->str, new_capacity * sizeof(char));
    t->str_capacity = new_capacity;
}

struct tokenized_str_t *tokenized_str_t_malloc() 
{
    struct tokenized_str_t *ts = (struct tokenized_str_t *) malloc(sizeof(struct tokenized_str_t));

    ts->tokens = (struct token_t **) malloc(STARTING_TOKENS * sizeof(struct token_t *));
    for (int i = 0; i < STARTING_TOKENS; i++)
        ts->tokens[i] = token_t_malloc();

    ts->size = 0;
    ts->capacity = STARTING_TOKENS;

    return ts;
}

void tokenized_str_t_free(struct tokenized_str_t *ts)
{
    for (size_t i = 0; i < ts->capacity; i++)
        token_t_free(ts->tokens[i]);

    free(ts->tokens);
    free(ts);
}

void tokenized_str_t_resize(struct tokenized_str_t *ts, size_t new_capacity)
{
    //TODO: only works when INCREASING size
    ts->tokens = (struct token_t **) realloc(ts->tokens, new_capacity * sizeof(struct token_t *));
    for (size_t i = ts->capacity; i < new_capacity; i++)
        ts->tokens[i] = token_t_malloc();

    ts->capacity = new_capacity;
}

/* clears the object and prepares it for a new loop */
void tokenized_str_t_clear(struct tokenized_str_t *ts)
{
    for (size_t i = 0; i < ts->size + 1; i++) {
        ts->tokens[i]->str_len = 0;
        ts->tokens[i]->type = O_NONE;
    }

    ts->size = 0;
}


void token_t_append_char(struct token_t *t, char c)
{
    if (t->str_len >= t->str_capacity)
        token_t_resize(t, t->str_capacity + 32);

    t->str[t->str_len++] = c;
}

void token_t_append_str(struct token_t *t, char *str)
{
    while (*str != 0)
        token_t_append_char(t, *str++);
}

void token_t_done(struct token_t *t)
{
    /* terminate string */
    token_t_append_char(t, 0);
    /* remove leading and trailing spaces */
    t->str_start = trim_edge(t->str, ' ');
}

void token_t_pop_char(struct token_t *t)
{
    if (t->str_len > 0)
        t->str[--(t->str_len)] = 0;
}

void tokenized_str_t_append_char(struct tokenized_str_t *ts, char c)
{
    token_t_append_char(ts->tokens[ts->size], c);
}

struct token_t *tokenized_str_t_next(struct tokenized_str_t *ts)
{
    if (++(ts->size) >= ts->capacity)
        tokenized_str_t_resize(ts, ts->capacity + 32);

    return ts->tokens[ts->size];
}

void token_t_print(struct token_t *t)
{
    char type[32];
    if (t->type == O_NONE)
        strcpy(type, "O_NONE");
    else
        strcpy(type, operands_str[t->type]);

    print_debug("TOKEN_T: str: '%s', type: '%s', str_len: '%ld', str_capacity: '%ld'",\
           t->str, type, t->str_len, t->str_capacity);
}

/* just for debugging */
void tokenized_str_t_print(struct tokenized_str_t *ts)
{
    print_debug("metadata: total tokens: %ld, total tokens allocated: %ld\n\n", ts->size + 1, ts->capacity);

    for (size_t i = 0; i < ts->size + 1; i++) {
        printf("num: '%ld', ", i);
        token_t_print(ts->tokens[i]);
        printf("\n");
    }
}

operands_t which_operand(bool candidates[TOTAL_OPERANDS])
{
    for (int i = 0; i < TOTAL_OPERANDS; i++) {
        if (candidates[i] == true)
            return operands[i];
    }
    return O_NONE;
}

int update_candidates(char c, size_t pos, bool candidates[TOTAL_OPERANDS], int *total_candidates)
{
    for (int i = 0; i < TOTAL_OPERANDS; i++) {
        if (*total_candidates == 0)
            return 0;

        if (candidates[i] == true && operands_str[i][pos] != c) {
            candidates[i] = false;
            *total_candidates -= 1;
        }
    }
    return *total_candidates;
}

void print_syntax_error(const char *buf_start, char *buf_err, char *msg)
{
    fprintf(stderr, "valery: syntax error near: '%c'\n", *buf_err);
    fprintf(stderr, "%s\n", buf_start);

    int offset = buf_err - buf_start - 1 > 0 ? buf_err - buf_start - 1: 0;
    for (int i = 0; i < offset; i++)
        fprintf(stderr, " ");
    fprintf(stderr, "^ %s\n", msg);
}

int tokenize(struct tokenized_str_t *ts, struct env_t *env, char *buffer)
{
    char c;
    /* always points to first address of buffer */
    const char *buf_start = buffer;
    /*
     * keys in list are operands_t integral constants.
     * values in list representing if token can be a possible operand for the given key.
     */
    bool candidates[TOTAL_OPERANDS];
    /* amount of values in candidates set to true */
    int total_candidates;
    size_t candidate_len = 0;                  /* keeps track of length of tokens that are possible operands */
    token_t *t = ts->tokens[ts->size]; /* the current token we are modifying */
    unsigned int p_flags = 0;

    while ((c = *buffer++) != 0) {
        /* reset possible candidates to all be true */
        memset(candidates, true, TOTAL_OPERANDS);
        total_candidates = TOTAL_OPERANDS;

        /* always add escaped character to token, no exception */
        if (p_flags & PF_ESCAPE) {
            p_flags ^= PF_ESCAPE;
            token_t_append_char(t, c);
            continue;
        }

        if (!special_char(env, t, c, &buffer, &p_flags))
            continue;

        if (!(p_flags & PF_QUOTE) && update_candidates(c, 0, candidates, &total_candidates)) {
            /* as the current char can be an operand, the current token is done and can be finalized */
            if (t->str_len != 0) {
                token_t_done(t);
                /* returns a pointer to the next token_t in ts->tokens */
                t = tokenized_str_t_next(ts);
            }

            do {
                token_t_append_char(t, c);
                update_candidates(c, candidate_len++, candidates, &total_candidates);

                /* operand is determined, add it and continue */
                if (total_candidates == 1) {
                    t->type = which_operand(candidates);
                    break;
                }

                /*
                 * if total_candidates is 0 then there is either a syntax error or token[:-1] is valid .
                 * f.ex: buffer = "ls | grep .c".
                 * first token will be "ls ".
                 * second token will then first be "|", which is indeterminate as operand can be
                 * either "|" or "||" depending on next char.
                 * after adding next char, token is now "| ". This is an invalid token, so remove previous char,
                 * and we get that the token is "|" which is a valid operand.
                 */
                if (total_candidates == 0) {
                    /* remove prev char */
                    token_t_pop_char(t);
                    buffer--;

                    for (int i = 0; i < TOTAL_OPERANDS; i++) {
                        if (strcmp(operands_str[i], t->str) == 0) {
                            t->type = operands[i];
                            goto finished_op;
                        }
                    }

                    /* execution enters here means operand was expected, but not found, i.e syntax error */
                    print_syntax_error(buf_start, buffer, "unexpected token");
                    return -1;
                }
            } while ((c = *buffer++) != 0);

        finished_op:
            /* only finalize token if we broke out of the loop (i.e operand was found and buffer not ended) */
            if (*buffer != 0) {
                token_t_done(t);
                t = tokenized_str_t_next(ts);
            }
            candidate_len = 0;

        } else {
            token_t_append_char(t, c);
        }

    }

    /*
     * finalize last token.
     * if it has O_NONE type then it has not been already finalized.
     * if skip flag is set then there was no closing qoutation mark, so throw syntax error 
     */
    token_t_done(t);

    if (p_flags & PF_QUOTE) {
        print_syntax_error(buf_start, buffer, "expected \"");
        return -1;
    } else if (p_flags & PF_ESCAPE) {
        print_syntax_error(buf_start, buffer - 1, "dangling escape character");
        return -1;
    } else if (which_operand(candidates) != O_NONE) {
        //TODO: does not work properly
        //print_syntax_error(buf_start, buffer - 2, "excpected another token after this operand");
        //return -1;
    }

    return 0;
}

bool special_char(struct env_t *env, struct token_t *t, char c, char **buffer, unsigned int *p_flags)
{
    char env_key[MAX_ENV_LEN];
    int pos = 0;
    switch (c) {
        /* do not parse chars inside qoutation marks, unless qoutation mark is escaped with backslash */
        case '"':
            if (*p_flags & PF_QUOTE)
                *p_flags ^= PF_QUOTE;

            else
                *p_flags |= PF_QUOTE;
            break;

        /* add flag that will*/
        case '\\':
            *p_flags |= PF_ESCAPE;
            break;

        /* replace with environment variable value */
        case '$':
            while ((c = **buffer) != 0) {
                /* environment variable keys can only contain numbers, uppercase letters, and underscores */
                if ((c >= ASCII_A && c <= ASCII_Z) || (c >= ASCII_A + 0x20 && c <= ASCII_Z + 0x20) ||
                    (c >= ASCII_0 && c <= ASCII_9) || c == ASCII_UNDERSCORE) {
                    env_key[pos++] = c;
                    (*buffer)++;
                } else {
                    break;
                }
            }
            env_key[pos] = 0;
            char *env_value = env_get(env, env_key);

            if (env_value != NULL)
                token_t_append_str(t, env_value);
            break;

        /* expand "." into current working directory */
        case '.':
            /* ignore */
            if (*p_flags & PF_QUOTE || *p_flags & PF_ESCAPE)
                return true;

            //if (peek(*buffer) == '.') {
            //}
            //break;
            printf("");

            char *PWD = env_get(env, "PWD");
            /* copy pwd into token */
            if (PWD != NULL)
                token_t_append_str(t, PWD);

            break;

        default:
            return true;
    }

    return false;
}

char *trim_edge(char *str, char c)
{
    char *str_cpy = str;
    char *str_start;

    /* trim all chars from start of string */
    while (*str_cpy == c) str_cpy++;
    str_start = str_cpy;

    /* move pointer to end of str */
    while (*str_cpy != 0) str_cpy++;

    /* trim all chars from end of string */
    while (*--str_cpy == c && str_cpy != str);
    /* terminate string after on match */
    *(++str_cpy) = 0;

    return str_start;
}

char peek(char *buffer)
{
    if (++(*buffer) == 0) {
        buffer--;
        return -1;
    } else
        return *buffer--;
}
