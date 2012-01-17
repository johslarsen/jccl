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
	void		*item;
};

struct Dlist {
	Dlist_node 	*head;
	int 		numitems;
};


Dlist *dlist_init(void)
{
	Dlist *list = malloc(sizeof(*list)); // new list
	if (list == NULL) {
		return NULL;
	}

	list->head = NULL;
	list->numitems = 0;

	return list;
}


int dlist_free(Dlist *list)
{
	if (list == NULL) {
		return EINVAL;
	}

	// remove each node
	while (list->head != NULL) {
		dlist_pop(list);
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

	Dlist_node *node = dlist_addnode(item);
	if (node == NULL) {
		return errno;
	}

	node->next = list->head;
	list->head = node;
	list->numitems++;

	return 0;
}


int dlist_append(Dlist *list, void *item)
{
	if (list == NULL) {
		return EINVAL;
	}

	Dlist_node *node = dlist_addnode(item);
	if (node == NULL) {
		return errno;
	}

	if (list->head == NULL) {
		// empty list
		list->head = node;
	} else {
		// nonempty list
		Dlist_node *np = list->head; // node pointer

		// traverse to last node
		while(np->next != NULL) {
			np = np->next;
		}
		np->next = node;
	}

	list->numitems++;

	return 0;
}


int dlist_remove(Dlist *list, void *item)
{
	if (list == NULL)
		return EINVAL;

	Dlist_node *np = NULL; // node pointer
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
		Dlist_node *pnp = list->head; // previous node pointer

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
	dlist_freenode(np);
	list->numitems--;
	return 0;
}


void *dlist_pop(Dlist *list)
{
	if (list == NULL || list->head == NULL) {
		return NULL;
	}

	Dlist_node *np = list->head;
	list->head = np->next;

	void *item = np->item;
	list->numitems--;
	free(np);

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
