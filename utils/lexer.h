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

#ifndef LEXER
#define LEXER

#define DEFAULT_TOKEN_SIZE 512
#define STARTING_TOKENS 8


/* types */
typedef struct tokens_t {
    char **token_arr;
    size_t *allocated_size;
    size_t i;
    size_t len;
} tokens_t;

/* functions */

/* returns a new token type with default malloced space */
struct tokens_t *new_tokens_t();

void free_tokens_t(struct tokens_t *tokens);

/* uses tokens->i to determine what token to have its memory space increased */
void increase_token_size(struct tokens_t *tokens, size_t new_size);

/* does not check if new_len >= old_len. Use precaution! */
void increase_tokens_amount(struct tokens_t *tokens, size_t new_len);

/*
 * splits input buffer by " "/whitespace delimiter.
 * stores tokens into tokens->token_arr and increases size of memory
 * for individual tokens and token_arr double pointer when necessary.
 */
void tokenize(struct tokens_t *tokens, char *buf);

#endif
