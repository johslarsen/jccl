#ifndef DLIST_H_
#define DLIST_H_

/* Double linked list interface */

typedef struct Dlist Dlist;

/* Create new list */
Dlist *dlist_init(void);

/* Free list.
 * All nodes are freed, but items pointed to by nodes are preserved. */
int dlist_free(Dlist *list);

/* Insert item first in list O=1 */
int dlist_prepend(Dlist *list, void *item);

/* Insert item last in list O=1 */
int dlist_append(Dlist *list, void *item);

/* Remove object from list. O=n, n = index of item */
int dlist_remove(Dlist *list, void *item);

/* Pop the list item off the list. O=1
 * The node that points to the item popped will be removed */
void *dlist_pop(Dlist *list);

/* Shift the first item off the list. O=1
 * The node that points to the item shifted will be removed */
void *dlist_shift(Dlist *list);

/* Return # of items in list. O=1 */
int dlist_size(Dlist *list);


/* Double linked list iterator interface */

typedef struct Dlist_iterator Dlist_iterator;

/* Create new list iterator */
Dlist_iterator *dlist_iterator_init(Dlist *list);

/* Free iterator */
int dlist_iterator_free(Dlist_iterator *li);

/* Move iterator to next item and return current */
void *dlist_iterator_next(Dlist_iterator *li);

/* Let iterator point to first item in list */
int dlist_iterator_reset(Dlist_iterator *li);

#endif /*DLIST_H_*/
