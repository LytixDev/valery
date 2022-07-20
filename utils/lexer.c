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

#include "lexer.h"

/*
 * enum representation is found in lexer.h.
 * comment dextral to the str is the integral constant.
 * since first value in enum is 'O_NONE' = 0, use 'O_<x>' - 1 to get str
 * representation given an integral constant.
 */
const char *operands_str[] = {
    "|",  /* O_PIPE */
    "||", /* O_OR   */
    "&&", /* O_AND  */
    ">",  /* O_RE   */
    ">>"  /* O_APP  */
};

const operands_t operands[] = {
    O_PIPE,
    O_OR,
    O_AND,
    O_RE,
    O_APP
};


//void tokenize(struct tokens_t *tokens, char *buf)
    

//{
//    const char delim[] = " ";
//    char *token = strtok(buf, delim);
//    size_t token_len;
//
//    while (token != NULL) {
//        /* check if space for token and if and more memory needs to be allocated */
//        if (tokens->i >= tokens->len) {
//            increase_tokens_amount(tokens, tokens->len + 32);
//        }
//
//        token_len = 0;
//        do {
//            tokens->token_arr[tokens->i][token_len] = *token;
//            if (token_len++ == tokens->allocated_size[tokens->i])
//                increase_token_size(tokens, token_len + 32);
//        } while (*(++token) != 0);
//
//        /* add null byte */
//        tokens->token_arr[tokens->i][token_len] = 0;
//
//        tokens->token_type[tokens->i] = get_token_operand(tokens->token_arr[tokens->i]);
//        tokens->i++;
//        token = strtok(NULL, delim);
//    }
//
//}

struct token_t *token_t_malloc()
{
    struct token_t *t = (struct token_t *) malloc(sizeof(struct token_t));
    t->str = (char *) malloc(DEFAULT_TOKEN_SIZE * sizeof(char));
    t->type = O_NONE;  /* set default type to none i.e, token is a program name */
    t->str_len = 0;
    t->str_allocated = DEFAULT_TOKEN_SIZE;

    return t;
}

void token_t_free(struct token_t *t)
{
    if (t == NULL)
        return;

    free(t->str);
    free(t);
}

void token_t_resize(struct token_t *t, size_t new_size)
{
    t->str = (char *) realloc(t->str, new_size * sizeof(char));
    t->str_allocated = new_size;
}


struct tokenized_str_t *tokenized_str_t_malloc() 
{
    struct tokenized_str_t *ts = (struct tokenized_str_t *) malloc(sizeof(struct tokenized_str_t));

    ts->tokens = (struct token_t **) malloc(STARTING_TOKENS * sizeof(struct token_t *));
    for (int i = 0; i < STARTING_TOKENS; i++)
        ts->tokens[i] = token_t_malloc();

    ts->total_tokens = 0;
    ts->tokens_allocated = STARTING_TOKENS;

    return ts;
}

void tokenized_str_t_free(struct tokenized_str_t *ts)
{
    if (ts == NULL)
        return;

    for (size_t i = 0; i < ts->tokens_allocated; i++)
        token_t_free(ts->tokens[i]);

    free(ts);
}

void tokenized_str_t_resize(struct tokenized_str_t *ts, size_t new_size)
{
    ts->tokens = (struct token_t **) realloc(ts->tokens, new_size * sizeof(struct token_t *));
    for (size_t i = ts->tokens_allocated; i < new_size; i++)
        ts->tokens[i] = token_t_malloc();

    ts->tokens_allocated = new_size;
}

/* clears the object and prepares it for a new loop */
void tokenized_str_t_clear(struct tokenized_str_t *ts)
{
    for (size_t i = 0; i < ts->total_tokens + 1; i++) {
        ts->tokens[i]->str_len = 0;
        ts->tokens[i]->type = O_NONE;
    }

    ts->total_tokens = 0;
}

void token_t_append_char(struct token_t *t, char c)
{
    if (t->str_len >= t->str_allocated)
        token_t_resize(t, t->str_allocated + 32);

    t->str[t->str_len++] = c;
}

void token_t_pop_char(struct token_t *t)
{
    if (t->str_len > 0)
        t->str[--(t->str_len)] = 0;
}

void tokenized_str_t_append_char(struct tokenized_str_t *ts, char c)
{
    token_t_append_char(ts->tokens[ts->total_tokens], c);
}

/* adds sentinel value to token_t->str and increments total tokens */
void tokenized_str_t_finalize_token(struct tokenized_str_t *ts)
{
    if (ts->total_tokens == ts->tokens_allocated)
        tokenized_str_t_resize(ts, ts->tokens_allocated + 32);

    token_t_append_char(ts->tokens[ts->total_tokens++], 0);
}

/* just for debugging purpsos */
void tokenized_str_t_print(struct tokenized_str_t *ts)
{
    int type;
    printf("metadata: total tokens: %ld, total tokens allocated: %ld\n\n", ts->total_tokens + 1, ts->tokens_allocated);

    for (size_t i = 0; i < ts->total_tokens + 1; i++) {
        type = ts->tokens[i]->type - 1;
        if (type == -1)
            printf("token num %ld, token str: '%s', token type: O_NONE, token len: %ld, token allocated: %ld\n",\
               i, ts->tokens[i]->str, ts->tokens[i]->str_len,\
               ts->tokens[i]->str_allocated);
        else
            printf("token num %ld, token str: '%s', token type: %s, token len: %ld, token allocated: %ld\n",\
               i, ts->tokens[i]->str, operands_str[ts->tokens[i]->type - 1], ts->tokens[i]->str_len,\
               ts->tokens[i]->str_allocated);
    }
}


bool bool_in_list(bool *list, size_t len, bool item)
{
    for (size_t i = 0; i < len; i++) {
        if (list[i] == item)
            return true;
    }
    return false;
}

int occurence_in_list(bool *list, size_t len, bool item)
{
    int s = 0;
    for (size_t i = 0; i < len; i++) {
        if (list[i] == item)
            s++;
    }
    return s;
}

/* returns first delim, therefore will not work if multiple delims are valid */
// jank but werks
operands_t which_delim(bool list[TOTAL_OPERANDS])
{
    for (int i = 0; i < TOTAL_OPERANDS; i++) {
        if (list[i] == true)
            return operands[i];
    }
    return O_NONE;
}


bool possible_delims(char c, size_t pos, bool pd[TOTAL_OPERANDS])
{
    for (int i = 0; i < TOTAL_OPERANDS; i++) {
        if (!bool_in_list(pd, TOTAL_OPERANDS, true))
            return false;

        if (operands_str[i][pos] != c) {
            pd[i] = false;
        }
    }

    return bool_in_list(pd, TOTAL_OPERANDS, true);
}

void tokenize(struct tokenized_str_t *ts, char *buffer)
{
    // TODO: remove
    char c;
    bool pd[TOTAL_OPERANDS];
    size_t token_len = 0;

    while ((c = *buffer++) != 0) {
        memset(pd, true, TOTAL_OPERANDS);

        if (possible_delims(c, 0, pd)) {
            /* is this too much of a black box for our purposes? */
            tokenized_str_t_finalize_token(ts);
            tokenized_str_t_append_char(ts, c);

            while ((c = *buffer++) != 0) {
                tokenized_str_t_append_char(ts, c);
                possible_delims(c, token_len++, pd);
                int len = occurence_in_list(pd, TOTAL_OPERANDS, true);

                /* if len is 1 then operand is determined and we can add it and continue */
                if (len == 1) {
                    ts->tokens[ts->total_tokens]->type = which_delim(pd);
                    break;
                }

                /* if len is 0 then there is either a syntax error or token minus current char is valid operand */
                if (len == 0) {
                    /* remove prev char */
                    token_t_pop_char(ts->tokens[ts->total_tokens]);

                    bool found = false;
                    for (int i = 0; i < TOTAL_OPERANDS; i++) {
                        if (strcmp(operands_str[i], ts->tokens[ts->total_tokens]->str) == 0) {
                            ts->tokens[ts->total_tokens]->type = operands[i];
                            found = true;
                            break;
                        }
                    }

                    if (found) {
                        buffer--;
                        break;
                    } else {
                        printf("SYNTAX ERROR near: '%c'\n", c);
                        return;
                    }
                }
                /* len is greater than 1 and the operand is ambigious so we continue */
            }

            tokenized_str_t_finalize_token(ts);
            token_len = 0;

        } else {
            tokenized_str_t_append_char(ts, c);
        }
    }
    /* finalize last token. if it has O_NONE type then it has not been finalized */
    if (ts->tokens[ts->total_tokens]->type == O_NONE) {
        /* increments total tokens */
        tokenized_str_t_finalize_token(ts);
        /* revert incrementation; no more tokens will be added */
        ts->total_tokens--;
    }
}

void trim_spaces(struct tokenized_str_t *ts)
{
    // TODO: this is janky
    /* removes any leading and trailing spaces from the tokens */

    char *str_ptr;
    char *str_start;
    char *str_end;
    char str_cpy[1024];
    char c;
    /* not memory safe */
    for (int i = 0; i < ts->total_tokens + 1; i++) {
        /* only do if type is O_NONE ? */
        str_ptr = ts->tokens[i]->str;
        str_start = str_ptr;
        str_end = NULL;

        /* if first char is space, set str start to first char that is not space */
        if ((c = *str_ptr++) == ' ') {
            while ((c = *str_ptr) != 0) {
                if (c != ' ') {
                    str_start = str_ptr;
                    break;
                }
                str_ptr++;
            }
        }
        /* move str_ptr to end of str */
        while ((c = *str_ptr++) != 0) continue;
        str_ptr--;
        str_ptr--;
        //printf("END: '%c'\n", *str_ptr);

        if ((c = *str_ptr--) == ' ') {
            //printf("TRUE");
            while ((c = *str_ptr--) != 0) {
                if (c != ' ') {
                    str_end = str_ptr + 2;
                    break;
                }
            }
        }

        /* SUPER JANK */
        size_t pos = 0;
        while ((c = *str_start++) != 0) {
            str_cpy[pos++] = c;
            if (str_start == str_end)
                break;
        }
        str_cpy[pos] = 0;

        //printf("'%s'\n", str_cpy);
        strcpy(ts->tokens[i]->str, str_cpy);
        ts->tokens[i]->str_len = strlen(str_cpy);

    }

}

/*
enum operands_t get_token_operand(char *token)
{
    if (strcmp(token, operands_str[0]) == 0)
        return O_PIPE;

    if (strcmp(token, operands_str[1]) == 0)
        return O_OR;

    if (strcmp(token, operands_str[2]) == 0)
        return O_AND;

    if (strcmp(token, operands_str[3]) == 0)
        return O_RE;

    if (strcmp(token, operands_str[4]) == 0)
        return O_APP;
    
    return O_NONE;
}
*/
