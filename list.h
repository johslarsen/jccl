#ifndef LIST_H_
#define LIST_H_

/* Single linked list interface */

typedef struct List List;

/* Create new list. ORDER=1
 * NULL on error */
List *list_init(void);

/* Free list. ORDER=n+1
 * return != 0 on error */
int list_free(List *list);

/* Insert item first in list. ORDER=1
 * return != 0 on error */
int list_prepend(List *list, void *item);

/* Insert item last in list. ORDER=n
 * return != 0 on error */
int list_append(List *list, void *item);

/* Remove object from list. ORDER=i, i=index of item
 * return != 0 on error */
int list_remove(List *list, void *item);

/* Shift the first item off the list. ORDER=1
 * The item shifted will be removed from the list
 * return NULL on error  or empty list */
void *list_shift(List *list);

/* Pop the last item off the list. ORDER=n
 * The item popped will be removed from the list
 * return NULL on error or empty list */
void *list_pop(List *list);

/* Return number of items in list. ORDER=1
 * return < 0 on error */
int list_size(List *list);


/* Single linked list iterator interface */

typedef struct List_iterator List_iterator;

/* Create new list iterator ORDER=1
 * return NULL on error */
List_iterator *list_iterator_init(List *list);

/* Free iterator ORDER=1
 * return != 0 on error */
int list_iterator_free(List_iterator *li);

/* Move iterator to next item and return current. ORDER=1
 * return NULL on error or end of list */
void *list_iterator_next(List_iterator *li);

/* Let iterator point to first item in list ORDER=1 
 * return != 0 on error */
int list_iterator_reset(List_iterator *li);

#endif /*LIST_H_*/
