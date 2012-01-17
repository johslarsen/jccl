#ifndef LIST_H_
#define LIST_H_

/* List interface */

typedef struct List List;

/* Create new list */
List *list_init(void);

/* Free list.
 * All nodes are freed, but items pointed to by nodes are preserved. */
int list_free(List *list);

/* Insert item first in list */
int list_prepend(List *list, void *item);

/* Insert item last in list */
int list_append(List *list, void *item);

/* Remove object from list */
int list_remove(List *list, void *item);

/* Pop the first item off the list
 * The node that points to the item popped will be removed */
void *list_pop(List *list);

/* Return # of items in list */
int list_size(List *list);


/* List iterator interface */

typedef struct List_iterator List_iterator;

/* Create new list iterator */
List_iterator *list_iterator_init(List *list);

/* Free iterator */
int list_iterator_free(List_iterator *li);

/* Move iterator to next item and return current */
void *list_iterator_next(List_iterator *li);

/* Let iterator point to first item in list */
int list_iterator_reset(List_iterator *li);

#endif /*LIST_H_*/
