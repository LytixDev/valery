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

#include "lexer.h"

const char *operands[] = {
    "|",  /* O_PIPE */
    "||", /* O_OR   */
    "&&", /* O_AND  */
    ">",  /* O_RE   */
    ">>"  /* O_APP  */
};

struct tokens_t *malloc_tokens_t()
{
    struct tokens_t *tokens = (tokens_t *) malloc(sizeof(tokens_t));
    tokens->token_arr = (char **) malloc(STARTING_TOKENS * sizeof(char *));
    tokens->allocated_size = (size_t *) malloc(STARTING_TOKENS * sizeof(size_t));
    tokens->token_type = (enum operands_t *) malloc(STARTING_TOKENS * sizeof(operands_t));

    for (size_t i = 0; i < STARTING_TOKENS;  i++){
        tokens->token_arr[i] = (char *) malloc(DEFAULT_TOKEN_SIZE * sizeof(char));
        tokens->allocated_size[i] = DEFAULT_TOKEN_SIZE;
        tokens->token_type[i] = O_NONE;
    }

    tokens->i = 0;
    tokens->len = STARTING_TOKENS;

    return tokens;
}

void free_tokens_t(struct tokens_t *tokens)
{
    if (tokens == NULL)
        return;

    for (size_t i = 0; i < tokens->len; i++)
        free(tokens->token_arr[i]);

    free(tokens->token_arr);
    free(tokens->allocated_size);
    free(tokens->token_type);
}

void increase_token_size(struct tokens_t *tokens, size_t new_size)
{
    size_t i = tokens->i;
    tokens->token_arr[i] = (char *) realloc(tokens->token_arr[i], new_size * sizeof(char));
    tokens->allocated_size[i] = new_size;
}

void increase_tokens_amount(struct tokens_t *tokens, size_t new_len)
{
    tokens->token_arr = (char **) realloc(tokens->token_arr, new_len * sizeof(char *));
    tokens->allocated_size = (size_t *) realloc(tokens->allocated_size, new_len * sizeof(size_t));
    tokens->token_type = (enum operands_t *) realloc(tokens->token_type, new_len * sizeof(operands_t));

    if (tokens->token_arr == NULL || tokens->allocated_size == NULL)
        printf("oh oh we in trouble\n");

    tokens->len = new_len;

    for (size_t i = tokens->len; i < new_len; i++) {
        tokens->token_arr[i] = (char *) malloc(DEFAULT_TOKEN_SIZE * sizeof(char));
        tokens->allocated_size[i] = DEFAULT_TOKEN_SIZE;
        tokens->token_type[i] = O_NONE;
    }
}

void tokenize(struct tokens_t *tokens, char *buf)
{
    const char delim[] = " ";
    char *token = strtok(buf, delim);

    while (token != NULL) {
        /* check if space for token and if and more memory needs to be allocated */
        if (tokens->i >= tokens->len) {
            increase_tokens_amount(tokens, tokens->len + 32);
        }

        /* TODO: avoid strlen by copying and reallocing (if necessary) in same pass ? */
        /* +1 to account for null byte */
        size_t token_len = strlen(token) + 1;
        if (token_len >= tokens->allocated_size[tokens->i]) {
            increase_token_size(tokens, token_len + 1);
        }

        tokens->token_type[tokens->i] = get_token_operand(token);
        strncpy(tokens->token_arr[tokens->i++], token, token_len + 1);
        token = strtok(NULL, delim);
    }

}

enum operands_t get_token_operand(char *token)
{
    if (strcmp(token, operands[0]) == 0)
        return O_PIPE;

    if (strcmp(token, operands[1]) == 0)
        return O_OR;

    if (strcmp(token, operands[2]) == 0)
        return O_AND;

    if (strcmp(token, operands[3]) == 0)
        return O_RE;

    if (strcmp(token, operands[4]) == 0)
        return O_APP;
    
    return O_NONE;
}
