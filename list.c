#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include "list.h"


/* Double linked list implementation */

struct list_node {
	struct list_node  *next, *previous;
	void              *item;
};

struct list {
	struct list_node  *head;
	struct list_node  *tail;
	int               nitem;
};


struct list *list_init(void)
{
	struct list *list = (struct list *) malloc(sizeof(*list));
	if (list == NULL) {
		return NULL;
	}

	list->head = NULL;
	list->tail = NULL;
	list->nitem = 0;

	return list;
}


void list_free(struct list *list)
{
	while (list_pop(list) != NULL) {
		;
	}

	free(list);
}


static struct list_node *list_addnode(struct list *list, void *item)
{
	struct list_node *node = (struct list_node *) malloc(sizeof(*node));
	if (node == NULL) {
		return NULL;
	}

	node->next = NULL;
	node->previous = NULL;
	node->item = item;

	list->nitem++;

	return node;
}


static void list_delnode(struct list *list, struct list_node *node)
{
	free(node);
	list->nitem--;
}


int list_prepend(struct list *list, void *item)
{
	if (list == NULL) {
		return EINVAL;
	}

	struct list_node *np = list_addnode(list, item);
	if (np == NULL) {
		return errno;
	}

	np->next = list->head;
	if (np->next == NULL) {
		list->tail = np;
	} else {
		np->next->previous = np;
	}

	list->head = np;

	return 0;
}


int list_append(struct list *list, void *item)
{
	if (list == NULL) {
		return EINVAL;
	}

	struct list_node *np = list_addnode(list, item);
	if (np == NULL) {
		return errno;
	}

	np->previous = list->tail;
	if (np->previous == NULL) {
		list->head = np;
	} else {
		np->previous->next = np;
	}

	list->tail = np;

	return 0;
}


int list_remove(struct list *list, void *item)
{
	if (list == NULL) {
		return EINVAL;
	}

	struct list_node *np = NULL; // node pointer
	if (list->head == NULL) {
		// empty list
		return EINVAL;
	}

	np = list->head;
	if (list->head->item == item) {
		list->head = np->next;
		if (list->head == NULL) {
			list->tail = NULL;
		} else {
			list->head->previous = NULL;
		}
	} else {
		while (np != NULL && np->item != item) {
			np = np->next;
		}

		if (np == NULL) {
			//item not in list
			return EINVAL;
		}

		np->previous->next = np->next;
		if (np->next == NULL) {
			list->tail = np->previous;
		} else {
			np->next->previous = np->previous;
		}
	}

	list_delnode(list, np);
	return 0;
}


void *list_pop(struct list *list)
{
	if (list == NULL || list->tail == NULL) {
		return NULL;
	}

	struct list_node *np = list->tail;
	if (np->previous == NULL) {
		list->head = NULL;
	} else {
		np->previous->next = NULL;
	}
	list->tail = np->previous;

	void *item = np->item;
	list_delnode(list, np);

	return item;
}


void *list_shift(struct list *list)
{
	if (list == NULL || list->head == NULL) {
		return NULL;
	}

	struct list_node *np = list->head;
	if (np->next == NULL) {
		list->tail = NULL;
	} else {
		np->next->previous = NULL;
	}
	list->head = np->next;

	void *item = np->item;
	list_delnode(list, np);

	return item;
}


int list_size(struct list *list)
{
	return list == NULL ? -EINVAL : list->nitem;
}


/* Double linked list iterator implementation */


struct list_iterator {
	struct list_node  *next, *previous;
	struct list       *list;
};


struct list_iterator *list_iterator_init(struct list *list)
{
	if (list == NULL) {
		return NULL;
	}

	struct list_iterator *li = (struct list_iterator *)malloc(sizeof(*li));
	if (li == NULL) {
		return NULL;
	}

	li->list = list;
	list_iterator_reset(li);

	return li;
}


void list_iterator_free(struct list_iterator *li)
{
	free(li);
}


void *list_iterator_next(struct list_iterator *li)
{
	if (li == NULL) {
		return NULL;
	}

	if (li->next == NULL) {
		return NULL;
	}

	void *item = li->next->item;

	li->previous = li->next;
	li->next = li->next->next;

	return item;
}


void *list_iterator_previous(struct list_iterator *li)
{
	if (li == NULL) {
		return NULL;
	}

	if (li->previous == NULL) {
		return NULL;
	}

	void *item = li->previous->item;

	li->next = li->previous;
	li->previous = li->previous->previous;

	return item;
}



int list_iterator_reset(struct list_iterator *li)
{
	if (li == NULL) {
		return EINVAL;
	}

	li->next = li->list->head;
	li->previous = li->list->tail;
	return 0;
}
