#ifndef SORT_H
#define SORT_H
#include <stddef.h>

/*
 * sort an array of integers so that odd numbers come before even numbers, the
 * rest of the order is undefined.
 *
 * answer to INF-1100 exam (2011-12-12) question 3 (a) "Skisser i pseudo-kode
 * en funksjon som organiserer tallene i A slik at oddetall kommer før partall."
 */
void sort_odd_even(int *a, int na);


/*
 * categorize some element a
 *
 * returns:
 *   n <-- 0 <= n < nbucket (supplied to bsort)
 */
typedef size_t (*bsort_categorizer)(const void *a);

/*
 * a generalized in-place bucket sorting algorithm.
 *
 * this is an exchange algorithm that is unstable (i.e. does not maintain
 * relative order). it has a worst case performance O(nmemb*(nbucket-1)) and a
 * best case performance O(nmemb) when the input is presorted. it is
 * non-recursive and being in-place it only requires O(1) additional space.
 *
 * it requires a non-NULL buckets that fits nbucket pointers. after completion
 * this contains pointers to the first element of the given category. being an
 * in-place algorithm categories will overlap, and empty categories points to
 * the same element as the next category or at the end of the base array.
 */
void bsort(void *base, size_t nmemb, size_t size, bsort_categorizer categorizer, size_t nbucket, void *buckets[]);

/*
 * bsort from base_unsorted and outwards.
 *
 * requires buckets initialized as it would be after bsort-ing upto, but not
 * including, base_unsorted. this function skips the initialization step, which
 * will scan from base until it finds the first unsorted element. its worst case
 * performance is O(m*nbucket), N=n+m with the first n element being sorted
 * followed by m unsorted elements. bsort would require an additional step that
 * scans through the first n elements.
 */
void bsort_partial(void *base, size_t nmemb, size_t size, bsort_categorizer categorizer, size_t nbucket, void *buckets[], void *base_unsorted);

#endif /* SORT_H */
