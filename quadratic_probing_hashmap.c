/*
 
Copyright (c) 2011, Willem-Hendrik Thiart
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * The names of its contributors may not be used to endorse or promote
      products derived from this software without specific prior written
      permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL WILLEM-HENDRIK THIART BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*/

#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <string.h>
#include <assert.h>

#include "quadratic_probing_hashmap.h"

/* when we call for more capacity */
#define SPACERATIO 0.5

static int __tombstone;

typedef struct hash_node_s hash_node_t;

struct hash_node_s
{
    void *key;                  // the refrence 
    void *val;                  // the data
};

static void __ensurecapacity(
    hashmapq_t * h
);

static int is_power_of_two(unsigned int x)
{
  return ((x != 0) && !(x & (x - 1)));
}

hashmapq_t *hashmapq_new(
    func_longhash_f hash,
    func_longcmp_f cmp,
    unsigned int initial_capacity
)
{
    hashmapq_t *h;

    assert(is_power_of_two(initial_capacity));

    h = calloc(1, sizeof(hashmapq_t));
    h->size = initial_capacity;
    h->array = calloc(h->size, sizeof(hash_node_t));
    h->hash = hash;
    h->compare = cmp;
    return h;
}

/**
 * @return number of items within hash */
int hashmapq_count(const hashmapq_t * h)
{
    return h->count;
}

/**
 * @return size of the array used within hash */
int hashmapq_size(
    hashmapq_t * h
)
{
    return h->size;
}

/**
 * Empty this hash. */
void hashmapq_clear(
    hashmapq_t * h
)
{
    int ii;

    for (ii = 0; ii < h->size; ii++)
    {
        hash_node_t *n;

        n = &((hash_node_t *) h->array)[ii];

        if (NULL == n->key)
            continue;

        if (n->key == (void*)&__tombstone)
        {
            n->key = NULL;
            continue;
        }

        n->key = NULL;
        h->count--;
        assert(0 <= h->count);
    }

    assert(0 == hashmapq_count(h));
}

/**
 * Free all the memory related to this hash. */
void hashmapq_free(
    hashmapq_t * h
)
{
    assert(h);
    hashmapq_clear(h);
}

/**
 * Free all the memory related to this hash.
 * This includes the actual h itself. */
void hashmapq_freeall(
    hashmapq_t * h
)
{
    assert(h);
    hashmapq_free(h);
    free(h);
}

/**
 * Get this key's value.
 * @return key's item, otherwise NULL */
void *hashmapq_get(
    hashmapq_t * h,
    const void *key
)
{
    unsigned int slot;
    hash_node_t *n;

    if (0 == hashmapq_count(h) || !key)
        return NULL;

    slot = h->hash(key);
    n = &((hash_node_t *) h->array)[slot % h->size];

    int new_slot;
    int i;

    for (i=0;;i++)
    {
        new_slot = (slot + (i/2) + (i * i)/2) % h->size;
        n = &((hash_node_t *) h->array)[new_slot];

        if (!n->key) break;
        if (n->key == (void*)&__tombstone) continue;

        if (0 == h->compare(key, n->key))
        {
            return (void *) n->val;
        }
    }

    return NULL;
}

/**
 * Is this key inside this map?
 * @return 1 if key is in hash, otherwise 0 */
int hashmapq_contains_key(
    hashmapq_t * h,
    const void *key
)
{
    return (NULL != hashmapq_get(h, key));
}

/**
 * Remove the value refrenced by this key from the hash. */
void hashmapq_remove_entry(
    hashmapq_t * h,
    hash_entry_t * entry,
    const void *k
)
{
    hash_node_t *n;
    int slot;
    int i;

    slot = h->hash(k);
    n = &((hash_node_t *) h->array)[slot % h->size];

    if (!n->key)
        goto notfound;

    for (i=0;;i++)
    {
        int new_slot;

        new_slot = (slot + (i/2) + (i * i)/2) % h->size;
        n = &((hash_node_t *) h->array)[new_slot];

        if (!n->key) goto notfound;
        if (n->key == (void*)&__tombstone) continue;

        if (0 == h->compare(k, n->key))
        {
            n->key = &__tombstone;
            h->count--;
            entry->key = n->key;
            entry->val = n->val;
            return;
        }
    }
   
notfound:
    entry->key = NULL;
    entry->val = NULL;
}

/**
 * Remove this key and value from the map.
 * @return value of key, or NULL on failure */
void *hashmapq_remove(
    hashmapq_t * h,
    const void *key
)
{
    hash_entry_t entry;

    hashmapq_remove_entry(h, &entry, key);
    return (void *) entry.val;
}

/**
 * Associate key with val.
 * Does not insert key if an equal key exists.
 * @return previous associated val; otherwise NULL */
void *hashmapq_put(
    hashmapq_t * h,
    void *k,
    void *v
)
{
    hash_node_t *n;
    int slot;

    if (!k || !v)
        return NULL;

    __ensurecapacity(h);

    slot = h->hash(k);

    int new_slot;
    int i;

    /* we are always at least half full
     * this guarantees we will be able to escape this loop */
    for (i=0;;i++)
    {
        new_slot = (slot + (i/2) + (i * i)/2) % h->size;
        n = &((hash_node_t *) h->array)[new_slot];

        if (!n->key || n->key == &__tombstone)
        {
            /* Don't double increment if we are replacing an item */
            if (!n->key)
            {
                h->slots_used += 1;
                h->count++;
            }

            n->key = k;
            n->val = v;
            return NULL;
        }
        else if (0 == h->compare(k, n->key))
        {
            void* old;

            old = n->val;
            n->val = v;
            return old;
        }
    }
}

/**
 * Put this key/value entry into the hash */
void hashmapq_put_entry(
    hashmapq_t * h,
    hash_entry_t * entry
)
{
    hashmapq_put(h, entry->key, entry->val);
}

/**
 * Increase hash capacity. */
void hashmapq_increase_capacity(hashmapq_t * h)
{
    hash_node_t *array_old;
    int ii, asize_old;

    /*  stored old array */
    array_old = h->array;
    asize_old = h->size;

    h->count = 0;
    h->slots_used = 0;
    h->size <<= 1;
    h->array = calloc(h->size, sizeof(hash_node_t));

    for (ii=0; ii < asize_old; ii++)
    {
        hash_node_t *n;
        
        n = &((hash_node_t *) array_old)[ii];

        if (!n->key || n->key == &__tombstone)
            continue;

        hashmapq_put(h, n->key, n->val);
    }

    free(array_old);
}

static void __ensurecapacity(
    hashmapq_t * h
)
{
    if ((float) h->slots_used / h->size < SPACERATIO)
    {
        return;
    }
    else
    {
        hashmapq_increase_capacity(h);
    }
}

void* hashmapq_iterator_peek(
    hashmapq_t * h,
    hashmapq_iterator_t * iter
)
{
    for (; iter->cur < h->size; iter->cur++)
    {
        hash_node_t *n;

        n = &((hash_node_t *) h->array)[iter->cur];

        if (n->key && n->key != &__tombstone)
            return n->key;
    }

    return NULL;

}

void* hashmapq_iterator_peek_value(
    hashmapq_t * h,
    hashmapq_iterator_t * iter)
{
    return hashmapq_get(h,hashmapq_iterator_peek(h,iter));
}

int hashmapq_iterator_has_next(
    hashmapq_t * h,
    hashmapq_iterator_t * iter
)
{
    return NULL != hashmapq_iterator_peek(h,iter);
}

/**
 * Iterate to the next item on a hash iterator
 * @return next item value from iterator */
void *hashmapq_iterator_next_value(
    hashmapq_t * h,
    hashmapq_iterator_t * iter
)
{
    void* k;
    
    k = hashmapq_iterator_next(h,iter);
    if (!k) return NULL;
    return hashmapq_get(h,k);
}

/**
 * Iterate to the next item on a hash iterator
 * @return next item key from iterator */
void *hashmapq_iterator_next(
    hashmapq_t * h,
    hashmapq_iterator_t * iter
)
{
    hash_node_t *n;

    assert(iter);

    for (; iter->cur < h->size; iter->cur++)
    {
        n = &((hash_node_t *) h->array)[iter->cur];

        if (!n->key || n->key == &__tombstone) continue;

        iter->cur++;
        return n->key;
    }

    return NULL;
}

/**
 * Initialise a new hash iterator over this hash
 * It is safe to remove items while iterating.
 */
void hashmapq_iterator(
    hashmapq_t * h __attribute__((__unused__)),
    hashmapq_iterator_t * iter
)
{
    iter->cur = 0;
}

/*--------------------------------------------------------------79-characters-*/
