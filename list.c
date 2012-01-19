#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include "list.h"


/* Singel linked list implementation */

typedef struct List_node List_node;
struct List_node {
	List_node	*next;
	void		*item;
};

struct List {
	List_node 	*head;
	int 		numitems;
};


List *list_init(void)
{
	List *list = malloc(sizeof(*list)); // new list
	if (list == NULL) {
		return NULL;
	}

	list->head = NULL;
	list->numitems = 0;

	return list;
}


int list_free(List *list)
{
	if (list == NULL) {
		return EINVAL;
	}

	// remove each node
	while (list_shift(list) != NULL) {
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

	List_node *node = list_addnode(item);
	if (node == NULL) {
		return errno;
	}

	node->next = list->head;
	list->head = node;
	list->numitems++;

	return 0;
}


int list_append(List *list, void *item)
{
	if (list == NULL) {
		return EINVAL;
	}

	List_node *node = list_addnode(item);
	if (node == NULL) {
		return errno;
	}

	if (list->head == NULL) {
		// empty list
		list->head = node;
	} else {
		// nonempty list
		List_node *np = list->head; // node pointer

		// traverse to last node
		while(np->next != NULL) {
			np = np->next;
		}
		np->next = node;
	}

	list->numitems++;

	return 0;
}


int list_remove(List *list, void *item)
{
	if (list == NULL)
		return EINVAL;

	List_node *np = NULL; // node pointer
	if (list->head == NULL) {
		// empty list
		return EINVAL;
	}

	if (list->head->item == item) {
		// item at head of list
		np = list->head;
		list->head = np->next;
	} else {
		// locate item in list
		List_node *pnp = list->head; // previous node pointer

		// traverse list to find item
		while (pnp->next != NULL && pnp->next->item != item) {
			pnp = pnp->next;
		}

		if (pnp->next == NULL) {
			//item not in list
			return EINVAL;
		}

		np = pnp->next;
		pnp->next = np->next;
	}

	// item located, remove it
	list_freenode(np);
	list->numitems--;
	return 0;
}


void *list_shift(List *list)
{
	if (list == NULL || list->head == NULL) {
		return NULL;
	}

	List_node *np = list->head;
	list->head = np->next;

	void *item = np->item;
	list->numitems--;
	list_freenode(np);

	return item;
}


void *list_pop(List *list)
{
	if (list == NULL || list->head == NULL) {
		return NULL;
	}

	List_node *np = list->head;
	if (np->next == NULL) {
		list->head = NULL;
	} else {
		while (np->next->next != NULL) {
			// traverse to the end of the list
			np = np->next;
		}
		List_node *pnp = np;
		np = pnp->next;
		pnp->next = NULL;
	}

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


/* Single linked list iterator implementation */


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
