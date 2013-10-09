typedef unsigned long (
    *func_longhash_f
)    (
    const void *
);

typedef long (
    *func_longcmp_f
)    (
    const void *,
    const void *
);

typedef struct
{
    void *key;                  // the refrence 
    void *val;                  // the data
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

int hashmapq_count(const hashmapq_t * hmap);

int hashmapq_size(
    hashmapq_t * hmap
);

void hashmapq_clear(
    hashmapq_t * hmap
);

void hashmapq_free(
    hashmapq_t * hmap
);

void hashmapq_freeall(
    hashmapq_t * hmap
);

void *hashmapq_get(
    hashmapq_t * hmap,
    const void *key
);

int hashmapq_contains_key(
    hashmapq_t * hmap,
    const void *key
);

void hashmapq_remove_entry(
    hashmapq_t * hmap,
    hash_entry_t * entry,
    const void *key
);

void *hashmapq_remove(
    hashmapq_t * hmap,
    const void *key
);

void *hashmapq_put(
    hashmapq_t * hmap,
    void *key,
    void *val
);

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

void *hashmapq_iterator_next(
    hashmapq_t * hmap,
    hashmapq_iterator_t * iter
);

void *hashmapq_iterator_next_value(
    hashmapq_t * hmap,
    hashmapq_iterator_t * iter);

void hashmapq_iterator(
    hashmapq_t * hmap,
    hashmapq_iterator_t * iter
);

void hashmapq_increase_capacity(hashmapq_t * hmap);
