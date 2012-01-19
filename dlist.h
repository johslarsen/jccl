#ifndef DLIST_H_
#define DLIST_H_

/* Double linked list interface */

typedef struct List List;

/* Create new list */
List *list_init(void);

/* Free list.
 * All nodes are freed, but items pointed to by nodes are preserved. */
int list_free(List *list);

/* Insert item first in list O=1 */
int list_prepend(List *list, void *item);

/* Insert item last in list O=1 */
int list_append(List *list, void *item);

/* Remove object from list. O=n, n = index of item */
int list_remove(List *list, void *item);

/* Shift the first item off the list. O=1
 * The node that points to the item shifted will be removed */
void *list_shift(List *list);

/* Pop the list item off the list. O=1
 * The node that points to the item popped will be removed */
void *list_pop(List *list);

/* Return # of items in list. O=1 */
int list_size(List *list);


/* Double linked list iterator interface */

typedef struct List_iterator List_iterator;

/* Create new list iterator */
List_iterator *list_iterator_init(List *list);

/* Free iterator */
int list_iterator_free(List_iterator *li);

/* Move iterator to next item and return current */
void *list_iterator_next(List_iterator *li);

/* Let iterator point to first item in list */
int list_iterator_reset(List_iterator *li);

#endif /*DLIST_H*/
