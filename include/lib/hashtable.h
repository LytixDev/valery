/*
 *  Hashtable implementation in C.
 *  Part of valery.
 *
 *  Inspired by:
 *  Python's dict implementation.
 *      <https://github.com/python/cpython/blob/main/Objects/dictobject.c>
 *  Engineer Man's hashtable implementation.
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

#ifndef LIB_HASHTABLE
#define LIB_HASHTABLE

#include <stdlib.h>


/* types */
typedef struct ht_item_t {
    char *key;
    void *value;
    void (*free_func)(void *);
    struct ht_item_t *next;
} ht_item_t;


typedef struct ht_t {
    struct ht_item_t **items;
    size_t capacity;
    size_t *keys;   /* how many items that are stored per hash */
} ht_t;


/* functions */
struct ht_t *ht_malloc(size_t capacity);

/* frees the entire ht and all items associated with it */
void ht_free(struct ht_t *ht);

/*
 * allocates space a new ht_item_t, computes the hash, and slots the 
 * item into the given 'ht_t *ht' hashtable. Frees and overrides previous
 * item with if there is an item with the exact same key.
 */
void ht_set(struct ht_t *ht, char *key, void *value, size_t mem_size, void (*free_func)(void *));

/* returns the value corresponding to the given key */
void *ht_get(struct ht_t *ht, char *key);

/* returns the first item stored with the given hash argument */
struct ht_item_t *ht_geth(struct ht_t *ht, unsigned int hash);

/* removes and frees the item the hashtable */
void ht_rm(struct ht_t *ht, char *key);

#endif /* LIB_HASHTABLE */
