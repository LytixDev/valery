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

#include <stdbool.h>

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

void tokenize(struct lex_t *lx, char *source)
{
    char c;
    size_t start = 0;
    size_t cur = 0;
    size_t line = 1;

    while ((c = *source) != 0) {
        start = cur;
    }
}

void scan_token()
{
}
