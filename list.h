#ifndef LIST_H_
#define LIST_H_

/* Double linked list interface */

struct list *list_init(void);
void list_free(struct list *list);
int list_prepend(struct list *list, void *item);
int list_append(struct list *list, void *item);
int list_remove(struct list *list, void *item);
void *list_shift(struct list *list);
void *list_pop(struct list *list);
int list_size(struct list *list);


/* Double linked list iterator interface */

struct list_iterator *list_iterator_init(struct list *list);
void list_iterator_free(struct list_iterator *li);
void *list_iterator_next(struct list_iterator *li);
void *list_iterator_previous(struct list_iterator *li);
int list_iterator_reset(struct list_iterator *li);

#endif /*LIST_H_*/
