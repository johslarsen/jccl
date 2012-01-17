#ifndef DLIST_H_
#define DLIST_H_

/* Double linked list interface */

typedef struct Dlist Dlist;

/* Create new list */
Dlist *dlist_init(void);

/* Free list.
 * All nodes are freed, but items pointed to by nodes are preserved. */
int dlist_free(Dlist *list);

/* Insert item first in list */
int dlist_prepend(Dlist *list, void *item);

/* Insert item last in list */
int dlist_append(Dlist *list, void *item);

/* Remove object from list */
int dlist_remove(Dlist *list, void *item);

/* Pop the first item off the list
 * The node that points to the item popped will be removed */
void *dlist_pop(Dlist *list);

/* Return # of items in list */
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
