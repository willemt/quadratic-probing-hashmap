#ifndef QUADRATIC_PROBING_HASHMAP_H
#define QUADRATIC_PROBING_HASHMAP_H

typedef unsigned long (*func_longhash_f) (const void *);

typedef long (*func_longcmp_f) (const void *, const void *);

typedef struct
{
    void *key;
    void *val;
} hash_entry_t;

typedef struct
{
    /* this is inclusive of tombstones */
    int slots_used;
    /* number of items within the hashmap */
    int count;
    /* size of the array */
    int size;
    void *array;
    func_longhash_f hash;
    func_longcmp_f compare;
} hashmapq_t;

typedef struct
{
    int cur;
} hashmapq_iterator_t;

hashmapq_t *hashmapq_new(
    func_longhash_f hash,
    func_longcmp_f cmp,
    unsigned int initial_capacity
);

/**
 * @return number of items within hash */
int hashmapq_count(const hashmapq_t * hmap);

int hashmapq_size(
    hashmapq_t * hmap
);

/**
 * Empty this hash. */
void hashmapq_clear(
    hashmapq_t * hmap
);

/**
 * Free all the memory related to this hash. */
void hashmapq_free(
    hashmapq_t * hmap
);

/**
 * Free all the memory related to this hash.
 * This includes the actual h itself. */
void hashmapq_freeall(
    hashmapq_t * hmap
);

/**
 * Get this key's value.
 * @return key's item, otherwise NULL */
void *hashmapq_get(
    hashmapq_t * hmap,
    const void *key
);

/**
 * Is this key inside this map?
 * @return 1 if key is in hash, otherwise 0 */
int hashmapq_contains_key(
    hashmapq_t * hmap,
    const void *key
);

/**
 * Remove the value refrenced by this key from the hash. */
void hashmapq_remove_entry(
    hashmapq_t * hmap,
    hash_entry_t * entry,
    const void *key
);

/**
 * Remove this key and value from the map.
 * @return value of key, or NULL on failure */
void *hashmapq_remove(
    hashmapq_t * hmap,
    const void *key
);

/**
 * Associate key with val.
 * Does not insert key if an equal key exists.
 * @return previous associated val; otherwise NULL */
void *hashmapq_put(
    hashmapq_t * hmap,
    void *key,
    void *val
);

/**
 * Put this key/value entry into the hash */
void hashmapq_put_entry(
    hashmapq_t * hmap,
    hash_entry_t * entry
);

void* hashmapq_iterator_peek(
    hashmapq_t * hmap,
    hashmapq_iterator_t * iter);

void* hashmapq_iterator_peek_value(
    hashmapq_t * hmap,
    hashmapq_iterator_t * iter);

int hashmapq_iterator_has_next(
    hashmapq_t * hmap,
    hashmapq_iterator_t * iter
);

/**
 * Iterate to the next item on a hash iterator
 * @return next item key from iterator */
void *hashmapq_iterator_next(
    hashmapq_t * hmap,
    hashmapq_iterator_t * iter
);

/**
 * Iterate to the next item on a hash iterator
 * @return next item value from iterator */
void *hashmapq_iterator_next_value(
    hashmapq_t * hmap,
    hashmapq_iterator_t * iter);

/**
 * Initialise a new hash iterator over this hash
 * It is safe to remove items while iterating.  */
void hashmapq_iterator(
    hashmapq_t * hmap,
    hashmapq_iterator_t * iter
);

/**
 * Increase hash capacity. */
void hashmapq_increase_capacity(hashmapq_t * hmap);

#endif /* QUADRATIC_PROBING_HASHMAP_H */
