#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include "list.h"


/*
 * List implementation
 */

typedef struct List_node List_node;
struct List_node {
	List_node	*next;
	List_node	*prev;
	void		*item;
};

struct List {
	List_node 	*head;
	List_node	*tail;
	int 		numitems;
};


List *list_init(void)
{
	List *list = malloc(sizeof(*list)); // new list
	if (list == NULL) {
		return NULL;
	}

	list->head = NULL;
	list->tail = NULL;
	list->numitems = 0;

	return list;
}


int list_free(List *list)
{
	if (list == NULL) {
		return EINVAL;
	}

	// remove each node
	while (list_pop(list)) {
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
static List_node *list_addnode(void *item)
{
	List_node *node = malloc(sizeof(*node));
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
static int list_freenode(List_node *node)
{
	if (node == NULL) {
		return EINVAL;
	}

	free(node);
	return 0;
}


int list_prepend(List *list, void *item)
{
	if (list == NULL) {
		return EINVAL;
	}

	List_node *np = list_addnode(item);
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


int list_append(List *list, void *item)
{
	if (list == NULL) {
		return EINVAL;
	}

	List_node *np = list_addnode(item);
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


int list_remove(List *list, void *item)
{
	if (list == NULL) {
		return EINVAL;
	}

	List_node *np = NULL; // node pointer
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
	list_freenode(np);
	list->numitems--;
	return 0;
}


void *list_pop(List *list)
{
	if (list == NULL || list->tail == NULL) {
		return NULL;
	}

	List_node *np = list->tail;
	if (np->prev == NULL) {
		list->head = NULL;
	} else {
		np->prev->next = NULL;
	}
	list->tail = np->prev;

	void *item = np->item;
	list_freenode(np);
	list->numitems--;

	return item;
}


void *list_shift(List *list)
{
	if (list == NULL || list->head == NULL) {
		return NULL;
	}

	List_node *np = list->head;
	if (np->next == NULL) {
		list->tail = NULL;
	} else {
		np->next->prev = NULL;
	}
	list->head = np->next;

	void *item = np->item;
	list_freenode(np);
	list->numitems--;

	return item;
}


int list_size(List *list)
{
	if (list == NULL) {
		return -EINVAL;
	}

	return list->numitems;
}


/*
 * Iterator implementation
 */


struct List_iterator {
	List_node	*next;
	List		*list;
};


List_iterator *list_iterator_init(List *list)
{
	if (list == NULL) {
		return NULL;
	}

	List_iterator *li = malloc(sizeof(*li));
	if (li == NULL) {
		return NULL;
	}

	li->list = list;
	li->next = list->head;

	return li;
}


int list_iterator_free(List_iterator *li)
{
	if (li == NULL) {
		return EINVAL;
	}

	free(li);
	return 0;
}


void *list_iterator_next(List_iterator *li)
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


int list_iterator_reset(List_iterator *li)
{
	if (li == NULL) {
		return EINVAL;
	}

	li->next = li->list->head;
	return 0;
}
