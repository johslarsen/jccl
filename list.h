#ifndef LIST_H_
#define LIST_H_

/* Double linked list interface */


/*
 * list initializer.
 *
 * returns:
 *   error --> NULL
 *   --> *(new list)
 */
extern struct list *list_init(void);


/*
 * list destructor. if list == NULL it does nothing.
 */
extern void list_free(struct list *list);


/*
 * prepend an element to a list, i.e. add an element to the start of the list.
 * duplicates are also added to the list.
 *
 * returns:
 *   list == NULL || element == NULL --> EINVAL
 *   failure to create node --> errno
 *   --> 0
 */
extern int list_prepend(struct list *list, void *element);


/*
 * append an element to a list, i.e. add an element to the end of the list.
 * duplicates are also added to the list.
 *
 * returns:
 *   list == NULL || element == NULL --> EINVAL
 *   failure to create node --> errno
 *   --> 0
 */
extern int list_append(struct list *list, void *element);


/*
 * removes an element from a list.
 *
 * returns:
 *   list == NULL || element == NULL --> EINVAL
 *   element not int list --> EADDRNOTAVAIL
 *   --> 0
 */
extern int list_remove(struct list *list, void *element);


/*
 * shift an element from the start of the list, i.e. the element is removed
 * from the list and returned.
 *
 * returns:
 *   list == NULL --> NULL
 *   list_size(list) == 0 --> NULL
 *   --> *element
 */
extern void *list_shift(struct list *list);


/*
 * pop an element from the end of the list, i.e. the element is removed from
 * the list and returned.
 *
 * returns:
 *   list == NULL --> NULL
 *   list_size(list) == 0 --> NULL
 *   --> *element
 */
extern void *list_pop(struct list *list);


/*
 * number of elements stored in list.
 *
 * returns:
 *   list == NULL --> -EINVAL
 *   --> size
 */
extern int list_size(struct list *list);


/* Double linked list iterator interface */


/*
 * list iterator initializer.
 *
 * returns:
 *   list == NULL --> NULL
 *   error --> NULL
 *   --> *(new list_iterator)
 */
extern struct list_iterator *list_iterator_init(struct list *list);


/*
 * list iterator destructor. if list_iterator == NULL it does nothing.
 */
extern void list_iterator_free(struct list_iterator *li);


/*
 * progress a list iterator.
 *
 * returns:
 *   list_iterator == NULL --> NULL
 *   end of list --> NULL
 *   --> next element
 */
extern void *list_iterator_next(struct list_iterator *li);


/*
 * regress a list iterator.
 *
 * returns:
 *   list_iterator == NULL --> NULL
 *   start of list --> NULL
 *   --> previous element
 */
extern void *list_iterator_previous(struct list_iterator *li);


/*
 * reset a list iterator.
 *
 * returns:
 *   list_iterator == NULL --> EINVAL
 *   --> 0
 */
extern int list_iterator_reset(struct list_iterator *li);

#endif /*LIST_H_*/
