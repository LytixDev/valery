/*
 *  Helper functions for dealing with strings.
 *  Part of valery.
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

#include <string.h>


char *vstr_starts_with(const char *str, const char *substr)
{
    char *s1 = (char *) str;
    char *s2 = (char *) substr;
    char c1, c2;

    do {
        c1 = *s1++;
        c2 = *s2++;
        if (c2 == 0)
            return s1;
    } while (c1 == c2);

    return NULL;
}

void vstr_insert_c(char buf[], int len, char c, int index)
{
    char tmp[len];
    strncpy(tmp, buf, index);
    tmp[index] = c;
    strcpy(tmp + index + 1, buf + index); 
    strcpy(buf, tmp);
}

void vstr_remove_idx(char buf[], int len, int index)
{
    char tmp[len];
    strncpy(tmp, buf, index - 1);
    strcpy(tmp + index - 1, buf + index); 
    strcpy(buf, tmp);
}

int vstr_find_first_c(const char *str, char look_for)
{
    int found_pos = 0;

    while (str[found_pos] != 0) {
        if (str[found_pos] == look_for)
            return found_pos;
        found_pos++;
    }

    return -1;
}

char *vstr_trim_edges(char buf[], char c)
{
    char *str_cpy = buf;
    char *str_start;

    /* trim all chars from start of string */
    while (*str_cpy == c) str_cpy++;
    str_start = str_cpy;

    /* move pointer to end of str */
    while (*str_cpy != 0) str_cpy++;

    /* trim all chars from end of string */
    while (*--str_cpy == c && str_cpy != buf);
    /* terminate string after on match */
    *(++str_cpy) = 0;

    return str_start;
}
