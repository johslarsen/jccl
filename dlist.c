#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include "dlist.h"


/*
 * List implementation
 */

typedef struct Dlist_node Dlist_node;
struct Dlist_node {
	Dlist_node	*next;
	Dlist_node	*prev;
	void		*item;
};

struct Dlist {
	Dlist_node 	*head;
	Dlist_node	*tail;
	int 		numitems;
};


Dlist *dlist_init(void)
{
	Dlist *list = malloc(sizeof(*list)); // new list
	if (list == NULL) {
		return NULL;
	}

	list->head = NULL;
	list->tail = NULL;
	list->numitems = 0;

	return list;
}


int dlist_free(Dlist *list)
{
	if (list == NULL) {
		return EINVAL;
	}

	// remove each node
	while (dlist_pop(list)) {
		;
	}

	// remove the list
	free(list);
	return 0;
}


/*
 * Create a node.
 * Points to the original item.
 */
static Dlist_node *dlist_addnode(void *item)
{
	Dlist_node *node = malloc(sizeof(*node));
	if (node == NULL) {
		return NULL;
	}

	node->next = NULL;
	node->prev = NULL;
	node->item = item;

	return node;
}


/*
 * Frees the memory allocated by a node.
 * Item is preserved.
 */
static int dlist_freenode(Dlist_node *node)
{
	if (node == NULL) {
		return EINVAL;
	}

	free(node);
	return 0;
}


int dlist_prepend(Dlist *list, void *item)
{
	if (list == NULL) {
		return EINVAL;
	}

	Dlist_node *np = dlist_addnode(item);
	if (np == NULL) {
		return errno;
	}

	np->next = list->head;
	if (np->next == NULL) {
		list->tail = np;
	} else {
		np->next->prev = np;
	}

	list->head = np;
	list->numitems++;

	return 0;
}


int dlist_append(Dlist *list, void *item)
{
	if (list == NULL) {
		return EINVAL;
	}

	Dlist_node *np = dlist_addnode(item);
	if (np == NULL) {
		return errno;
	}

	np->prev = list->tail;
	if (np->prev == NULL) {
		list->head = np;
	} else {
		np->prev->next = np;
	}

	list->tail = np;
	list->numitems++;

	return 0;
}


int dlist_remove(Dlist *list, void *item)
{
	if (list == NULL) {
		return EINVAL;
	}

	Dlist_node *np = NULL; // node pointer
	if (list->head == NULL) {
		// empty list
		return EINVAL;
	}

	np = list->head;
	if (np->item == item) {
		// item at head of list
		list->head = np->next;
		if (list->head == NULL) {
			list->tail = NULL;
		} else {
			list->head->prev = NULL;
		}
	} else {
		// locate item in list

		// traverse list to find item
		while (np != NULL && np->item != item) {
			np = np->next;
		}

		if (np == NULL) {
			//item not in list
			return EINVAL;
		}
		np->prev->next = np->next;
		if (np->next == NULL) {
			list->tail = np->prev;
		} else {
			np->next->prev = np->prev;
		}
	}

	// item located, remove it
	dlist_freenode(np);
	list->numitems--;
	return 0;
}


void *dlist_pop(Dlist *list)
{
	if (list == NULL || list->tail == NULL) {
		return NULL;
	}

	Dlist_node *np = list->tail;
	if (np->prev == NULL) {
		list->head = NULL;
	} else {
		np->prev->next = NULL;
	}
	list->tail = np->prev;

	void *item = np->item;
	dlist_freenode(np);
	list->numitems--;

	return item;
}


void *dlist_shift(Dlist *list)
{
	if (list == NULL || list->head == NULL) {
		return NULL;
	}

	Dlist_node *np = list->head;
	if (np->next == NULL) {
		list->tail = NULL;
	} else {
		np->next->prev = NULL;
	}
	list->head = np->next;

	void *item = np->item;
	dlist_freenode(np);
	list->numitems--;

	return item;
}


int dlist_size(Dlist *list)
{
	if (list == NULL) {
		return -EINVAL;
	}

	return list->numitems;
}


/*
 * Iterator implementation
 */


struct Dlist_iterator {
	Dlist_node	*next;
	Dlist		*list;
};


Dlist_iterator *dlist_iterator_init(Dlist *list)
{
	if (list == NULL) {
		return NULL;
	}

	Dlist_iterator *li = malloc(sizeof(*li));
	if (li == NULL) {
		return NULL;
	}

	li->list = list;
	li->next = list->head;

	return li;
}


int dlist_iterator_free(Dlist_iterator *li)
{
	if (li == NULL) {
		return EINVAL;
	}

	free(li);
	return 0;
}


void *dlist_iterator_next(Dlist_iterator *li)
{
	if (li == NULL) {
		return NULL;
	}

	if (li->next == NULL) {
		return NULL;
	}

	void *item = li->next->item;

	li->next = li->next->next; // progress the iterator

	return item;
}


int dlist_iterator_reset(Dlist_iterator *li)
{
	if (li == NULL) {
		return EINVAL;
	}

	li->next = li->list->head;
	return 0;
}
