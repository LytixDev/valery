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

#ifndef VSTRING
#define VSTRING

/*
 * Helper functions when dealing with strings.
 *
 * NOTE: All functions that take 'char buf[]' WILL mutate the argument
 * passed in. All functions that take 'const char *str' leave the
 * argument unchanged.
 *
 * NOTE: Passing NULL as an argument is undefined behaviour.
 *
 * TODO: Functions are not memory safe.
 */

/* functions */

/* insert a char at any position into buf */
void vstr_insert_c(char buf[], int len, char c, int index);

/* removes char positioned at index from buf, and merges tail back in */
void vstr_remove_idx(char buf[], int len, int index);

/*
 * trims start and end of argument 'char c'.
 * returns a pointer to the new start of buf.
 * example:
 * vstr_trim_edges("111hello1", '1') -> hello.
 */
char *vstr_trim_edges(char buf[], char c);

/*
 * checks if str starts with substr.
 * if true, returns pointer to end of match in str.
 * else returns NULL.
 */
char *vstr_starts_with(const char *str, const char *substr);

/*
 * returns the first index of match.
 * returns -1 if no match was found
 */
int vstr_find_first_c(const char *str, char look_for);


#endif /* VSTRING */
