/*
 *  Hashtable implementantion in C.
 *  Part of valery.
 *
 *  Inspired by:
 *  Python's dict implementantion.
 *      <https://github.com/python/cpython/blob/main/Objects/dictobject.c>
 *  Engineer Man's hashtable implementantion.
 *      <https://github.com/engineer-man/youtube/tree/master/077>
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

// TODO/NOTE: still in experimental phase

#ifndef LIB_HASHTABLE
#define LIB_HASHTABLE

#include <stdlib.h>

/* variables */
#define TABLE_SIZE 100


/* types */
typedef struct ht_item_t {
    char *key;
    //TODO: use void pointer ?
    char *value;
    //void (*free_func)(void *);
    struct ht_item_t *next;
} ht_item_t;


typedef struct ht_t {
    ht_item_t **items;
    // TODO: dynamic size
    //size_t capacity;
} ht_t;


/* functions */
ht_t *ht_malloc();

void ht_free(struct ht_t *ht);

void ht_set(struct ht_t *ht, char *key, char *value);

char *ht_get(struct ht_t *ht, char *key);

void ht_rm(struct ht_t *ht, char *key);

void ht_dump(struct ht_t *ht);

/*
 * static functions:
 * hasher()
 * ht_item_malloc()
 */


#endif /* LIB_HASHTABLE */
