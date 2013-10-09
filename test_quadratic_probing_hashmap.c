#include <stdbool.h>
#include <assert.h>
#include <setjmp.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "CuTest.h"

#include "quadratic_probing_hashmap.h"

static unsigned long __uint_hash(
    const void *e1
)
{
    const long i1 = (unsigned long) e1;

    assert(i1 >= 0);
    return i1;
}

static long __uint_compare(
    const void *e1,
    const void *e2
)
{
    const long i1 = (unsigned long) e1, i2 = (unsigned long) e2;

//      return !(*i1 == *i2); 
    return i1 - i2;
}

void TesthashmapqQuadratic_New(
    CuTest * tc
)
{
    hashmapq_t *hm;

    hm = hashmapq_new(__uint_hash, __uint_compare, 8);

    CuAssertTrue(tc, 0 == hashmapq_count(hm));
    CuAssertTrue(tc, 8 == hashmapq_size(hm));
    hashmapq_freeall(hm);
}

void TesthashmapqQuadratic_Put(
    CuTest * tc
)
{
    hashmapq_t *hm;

    hm = hashmapq_new(__uint_hash, __uint_compare, 8);
    hashmapq_put(hm, (void *) 50, (void *) 92);

    CuAssertTrue(tc, 1 == hashmapq_count(hm));
    hashmapq_freeall(hm);
}

void TesthashmapqQuadratic_PutEnsuresCapacity(
    CuTest * tc
)
{
    hashmapq_t *hm;

    hm = hashmapq_new(__uint_hash, __uint_compare, 1);
    hashmapq_put(hm, (void *) 50, (void *) 92);
    hashmapq_put(hm, (void *) 51, (void *) 92);

    CuAssertTrue(tc, 2 == hashmapq_count(hm));
    CuAssertTrue(tc, 2 == hashmapq_size(hm));
    hashmapq_freeall(hm);
}

void TesthashmapqQuadratic_PutHandlesCollision(
    CuTest * tc
)
{
    hashmapq_t *hm;
    unsigned long val;

    hm = hashmapq_new(__uint_hash, __uint_compare, 4);
    hashmapq_put(hm, (void *) 1, (void *) 92);
    hashmapq_put(hm, (void *) 5, (void *) 93);

    CuAssertTrue(tc, 2 == hashmapq_count(hm));

    val = (unsigned long) hashmapq_get(hm, (void *) 5);
    CuAssertTrue(tc, 0 != val);
    CuAssertTrue(tc, val == 93);

    val = (unsigned long) hashmapq_get(hm, (void *) 1);
    CuAssertTrue(tc, 0 != val);
    CuAssertTrue(tc, val == 92);
    hashmapq_freeall(hm);
}

void TesthashmapqQuadratic_GetHandlesCollisionByTraversingChain(
    CuTest * tc
)
{
    hashmapq_t *hm;
    unsigned long val;

    hm = hashmapq_new(__uint_hash, __uint_compare, 4);
    hashmapq_put(hm, (void *) 1, (void *) 92);
    hashmapq_put(hm, (void *) 5, (void *) 93);

    CuAssertTrue(tc, 2 == hashmapq_count(hm));

    val = (unsigned long) hashmapq_get(hm, (void *) 5);
    CuAssertTrue(tc, 0 != val);
    CuAssertTrue(tc, val == 93);
    hashmapq_freeall(hm);
}

void TesthashmapqQuadratic_RemoveReturnsNullIfMissingAndTraversesChain(
    CuTest * tc
)
{
    hashmapq_t *hm;
    unsigned long val;

    hm = hashmapq_new(__uint_hash, __uint_compare, 4);
    hashmapq_put(hm, (void *) 1, (void *) 92);

    val = (unsigned long) hashmapq_remove(hm, (void *) 5);
    CuAssertTrue(tc, 0 == val);

    val = (unsigned long) hashmapq_remove(hm, (void *) 1);
    CuAssertTrue(tc, 0 != val);
    CuAssertTrue(tc, val == 92);

    hashmapq_freeall(hm);
}

void TesthashmapqQuadratic_RemoveHandlesCollision(
    CuTest * tc
)
{
    hashmapq_t *hm;
    unsigned long val;

    hm = hashmapq_new(__uint_hash, __uint_compare, 4);
    hashmapq_put(hm, (void *) 1, (void *) 92);
    hashmapq_put(hm, (void *) 5, (void *) 93);
    hashmapq_put(hm, (void *) 9, (void *) 94);

    val = (unsigned long) hashmapq_remove(hm, (void *) 5);
    CuAssertTrue(tc, 0 != val);
    CuAssertTrue(tc, val == 93);
    CuAssertTrue(tc, 2 == hashmapq_count(hm));

    val = (unsigned long) hashmapq_remove(hm, (void *) 1);
    CuAssertTrue(tc, 0 != val);
    CuAssertTrue(tc, val == 92);
    CuAssertTrue(tc, 1 == hashmapq_count(hm));

    hashmapq_freeall(hm);
}

void TesthashmapqQuadratic_PutEntry(
    CuTest * tc
)
{
    hashmapq_t *hm;
    hash_entry_t entry;

    hm = hashmapq_new(__uint_hash, __uint_compare, 8);
    entry.key = (void *) 50;
    entry.val = (void *) 92;
    hashmapq_put_entry(hm, &entry);
    CuAssertTrue(tc, 1 == hashmapq_count(hm));
    hashmapq_freeall(hm);
}

void TesthashmapqQuadratic_Get(
    CuTest * tc
)
{
    hashmapq_t *hm;

    unsigned long val;

    hm = hashmapq_new(__uint_hash, __uint_compare, 8);

    hashmapq_put(hm, (void *) 50, (void *) 92);

    val = (unsigned long) hashmapq_get(hm, (void *) 50);

    CuAssertTrue(tc, 0 != val);
    CuAssertTrue(tc, val == 92);
    hashmapq_freeall(hm);
}

void TesthashmapqQuadratic_ContainsKey(
    CuTest * tc
)
{
    hashmapq_t *hm;

    hm = hashmapq_new(__uint_hash, __uint_compare, 8);

    hashmapq_put(hm, (void *) 50, (void *) 92);

    CuAssertTrue(tc, 1 == hashmapq_contains_key(hm, (void*) 50));
    hashmapq_freeall(hm);
}


void TesthashmapqQuadratic_DoublePut(
    CuTest * tc
)
{
    hashmapq_t *hm;

    unsigned long val;

    hm = hashmapq_new(__uint_hash, __uint_compare, 8);
    hashmapq_put(hm, (void *) 50, (void *) 92);
    hashmapq_put(hm, (void *) 50, (void *) 23);
    val = (unsigned long) hashmapq_get(hm, (void *) 50);
    CuAssertTrue(tc, val == 23);
    CuAssertTrue(tc, 1 == hashmapq_count(hm));
    hashmapq_freeall(hm);
}

void TesthashmapqQuadratic_Get2(
    CuTest * tc
)
{
    hashmapq_t *hm;

    unsigned long val;

    hm = hashmapq_new(__uint_hash, __uint_compare, 8);
    hashmapq_put(hm, (void *) 50, (void *) 92);
    hashmapq_put(hm, (void *) 49, (void *) 91);
    hashmapq_put(hm, (void *) 48, (void *) 90);
    hashmapq_put(hm, (void *) 47, (void *) 89);
    hashmapq_put(hm, (void *) 46, (void *) 88);
    hashmapq_put(hm, (void *) 45, (void *) 87);
    val = (unsigned long) hashmapq_get(hm, (void *) 48);
    CuAssertTrue(tc, val == 90);
    hashmapq_freeall(hm);
}

void TesthashmapqQuadratic_IncreaseCapacityDoesNotBreakhashmapq(
    CuTest * tc
)
{
    hashmapq_t *hm;

    unsigned long val;

    hm = hashmapq_new(__uint_hash, __uint_compare, 4);
    hashmapq_put(hm, (void *) 1, (void *) 90);
    hashmapq_put(hm, (void *) 5, (void *) 91);
    hashmapq_put(hm, (void *) 2, (void *) 92);
    hashmapq_increase_capacity(hm);
    CuAssertTrue(tc, 3 == hashmapq_count(hm));
    val = (unsigned long) hashmapq_get(hm, (void *) 1);
    CuAssertTrue(tc, val == 90);
    val = (unsigned long) hashmapq_get(hm, (void *) 5);
    CuAssertTrue(tc, val == 91);
    val = (unsigned long) hashmapq_get(hm, (void *) 2);
    CuAssertTrue(tc, val == 92);
    hashmapq_freeall(hm);
}

void TesthashmapqQuadratic_Remove(
    CuTest * tc
)
{
    hashmapq_t *hm;

    unsigned long val;

    hm = hashmapq_new(__uint_hash, __uint_compare, 8);

    hashmapq_put(hm, (void *) 50, (void *) 92);

    val = (unsigned long) hashmapq_remove(hm, (void *) 50);

    CuAssertTrue(tc, 0 != val);
    CuAssertTrue(tc, val == 92);
    CuAssertTrue(tc, 0 == hashmapq_count(hm));
    hashmapq_freeall(hm);
}

void TesthashmapqQuadratic_ClearRemovesAll(
    CuTest * tc
)
{
    hashmapq_t *hm;

    unsigned long val;

    hm = hashmapq_new(__uint_hash, __uint_compare, 8);

    hashmapq_put(hm, (void *) 1, (void *) 92);
    hashmapq_put(hm, (void *) 2, (void *) 102);
    hashmapq_clear(hm);

    val = (unsigned long) hashmapq_get(hm, (void *) 1);
    CuAssertTrue(tc, 0 == val);
    CuAssertTrue(tc, 0 == hashmapq_count(hm));
    hashmapq_freeall(hm);
}

/* The clear function will need to clean the whole chain */
void TesthashmapqQuadratic_ClearHandlesCollision(
    CuTest * tc
)
{
    hashmapq_t *hm;

    hm = hashmapq_new(__uint_hash, __uint_compare, 4);

    hashmapq_put(hm, (void *) 1, (void *) 50);
    /* all of the rest cause collisions */
    hashmapq_put(hm, (void *) 5, (void *) 51);
    hashmapq_put(hm, (void *) 9, (void *) 52);
    hashmapq_clear(hm);
    CuAssertTrue(tc, 0 == hashmapq_count(hm));
    hashmapq_freeall(hm);
}

void TesthashmapqQuadratic_DoesNotHaveNextForEmptyIterator(
    CuTest * tc
)
{
    hashmapq_t *hm;
    hashmapq_iterator_t iter;

    hm = hashmapq_new(__uint_hash, __uint_compare, 8);
    hashmapq_iterator(hm, &iter);
    CuAssertTrue(tc, 0 == hashmapq_iterator_has_next(hm, &iter));
    hashmapq_freeall(hm);
}

void TesthashmapqQuadratic_RemoveItemDoesNotHaveNextForEmptyIterator(
    CuTest * tc
)
{
    hashmapq_t *hm;
    hashmapq_iterator_t iter;

    hm = hashmapq_new(__uint_hash, __uint_compare, 8);
    hashmapq_put(hm, (void *) 9, (void *) 52);
    hashmapq_remove(hm, (void *) 9);
    hashmapq_iterator(hm, &iter);
    CuAssertTrue(tc, 0 == hashmapq_iterator_has_next(hm, &iter));
    hashmapq_freeall(hm);
}

void TesthashmapqQuadratic_Iterate(
    CuTest * tc
)
{
    hashmapq_t *hm;
    hashmapq_t *hm2;
    hashmapq_iterator_t iter;
    void *key;

    hm = hashmapq_new(__uint_hash, __uint_compare, 8);
    hm2 = hashmapq_new(__uint_hash, __uint_compare, 8);

    hashmapq_put(hm, (void *) 50, (void *) 92);
    hashmapq_put(hm, (void *) 49, (void *) 91);
    hashmapq_put(hm, (void *) 48, (void *) 90);
    hashmapq_put(hm, (void *) 47, (void *) 89);
    hashmapq_put(hm, (void *) 46, (void *) 88);
    hashmapq_put(hm, (void *) 45, (void *) 87);
    hashmapq_put(hm2, (void *) 50, (void *) 92);


    hashmapq_put(hm2, (void *) 49, (void *) 91);
    hashmapq_put(hm2, (void *) 48, (void *) 90);
    hashmapq_put(hm2, (void *) 47, (void *) 89);
    hashmapq_put(hm2, (void *) 46, (void *) 88);
    hashmapq_put(hm2, (void *) 45, (void *) 87);

    hashmapq_iterator(hm, &iter);

    /*  remove every key we iterate on */
    while ((key = hashmapq_iterator_next(hm, &iter)))
    {
        CuAssertTrue(tc, NULL != hashmapq_remove(hm2, key));
    }

    /*  check if the hashmapq is empty */
    CuAssertTrue(tc, 0 == hashmapq_count(hm2));
    hashmapq_freeall(hm);
}

void TesthashmapqQuadratic_IterateHandlesCollisions(
    CuTest * tc
)
{
    hashmapq_t *hm, *hm2;

    hashmapq_iterator_t iter;

    void *key;

    hm = hashmapq_new(__uint_hash, __uint_compare, 4);
    hm2 = hashmapq_new(__uint_hash, __uint_compare, 4);

    hashmapq_put(hm, (void *) 1, (void *) 92);
    hashmapq_put(hm, (void *) 5, (void *) 91);
    hashmapq_put(hm, (void *) 9, (void *) 90);

    hashmapq_put(hm2, (void *) 1, (void *) 92);
    hashmapq_put(hm2, (void *) 5, (void *) 91);
    hashmapq_put(hm2, (void *) 9, (void *) 90);

    hashmapq_iterator(hm, &iter);

    /*  remove every key we iterate on */
    while ((key = hashmapq_iterator_next(hm, &iter)))
    {
        CuAssertTrue(tc, NULL != hashmapq_remove(hm2, key));
    }

    /*  check if the hashmapq is empty */
    CuAssertTrue(tc, 0 == hashmapq_count(hm2));
    hashmapq_freeall(hm);
}

void TesthashmapqQuadratic_IterateAndRemoveDoesntBreakIteration(
    CuTest * tc
)
{
    hashmapq_t *hm;
    hashmapq_t *hm2;
    hashmapq_iterator_t iter;
    void *key;

    hm = hashmapq_new(__uint_hash, __uint_compare, 8);
    hm2 = hashmapq_new(__uint_hash, __uint_compare, 8);

    hashmapq_put(hm, (void *) 50, (void *) 92);
    hashmapq_put(hm, (void *) 49, (void *) 91);
    hashmapq_put(hm, (void *) 48, (void *) 90);
    hashmapq_put(hm, (void *) 47, (void *) 89);
    hashmapq_put(hm, (void *) 46, (void *) 88);
    hashmapq_put(hm, (void *) 45, (void *) 87);
    /*  the following 3 collide: */
    hashmapq_put(hm, (void *) 1, (void *) 92);
    hashmapq_put(hm, (void *) 5, (void *) 91);
    hashmapq_put(hm, (void *) 9, (void *) 90);

    hashmapq_put(hm2, (void *) 50, (void *) 92);
    hashmapq_put(hm2, (void *) 49, (void *) 91);
    hashmapq_put(hm2, (void *) 48, (void *) 90);
    hashmapq_put(hm2, (void *) 47, (void *) 89);
    hashmapq_put(hm2, (void *) 46, (void *) 88);
    hashmapq_put(hm2, (void *) 45, (void *) 87);
    /*  the following 3 collide: */
    hashmapq_put(hm2, (void *) 1, (void *) 92);
    hashmapq_put(hm2, (void *) 5, (void *) 91);
    hashmapq_put(hm2, (void *) 9, (void *) 90);

    hashmapq_iterator(hm, &iter);

    /*  remove every key we iterate on */
    while ((key = hashmapq_iterator_next(hm, &iter)))
    {
        CuAssertTrue(tc, NULL != hashmapq_remove(hm2, key));
        hashmapq_remove(hm,key);
    }

    /*  check if the hashmapq is empty */
    CuAssertTrue(tc, 0 == hashmapq_count(hm2));
    hashmapq_freeall(hm);
    hashmapq_freeall(hm2);
}

