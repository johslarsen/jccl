#include "CuTest/CuTest.h"
#include "list.h"
#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>


/* Double linked list implementation */

struct list_node {
	struct list_node  *next, *previous;
	void              *element;
};

struct list {
	struct list_node  *head;
	struct list_node  *tail;
	int               nelement;
};


struct list *list_init(void) {
	struct list *list = (struct list *) malloc(sizeof(*list));
	if (list == NULL) {
		return NULL;
	}

	list->head = NULL;
	list->tail = NULL;
	list->nelement = 0;

	return list;
}


void list_free(struct list *list) {
	while (list_pop(list) != NULL) {
		;
	}

	free(list);
}


static struct list_node *list_addnode(struct list *list, void *element) {
	struct list_node *node = (struct list_node *) malloc(sizeof(*node));
	if (node == NULL) {
		return NULL;
	}

	node->next = NULL;
	node->previous = NULL;
	node->element = element;

	list->nelement++;

	return node;
}


static void list_delnode(struct list *list, struct list_node *node) {
	free(node);
	list->nelement--;
}


int list_prepend(struct list *list, void *element) {
	if (list == NULL || element == NULL) {
		return EINVAL;
	}

	struct list_node *np = list_addnode(list, element);
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


int list_append(struct list *list, void *element) {
	if (list == NULL || element == NULL) {
		return EINVAL;
	}

	struct list_node *np = list_addnode(list, element);
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


int list_remove(struct list *list, void *element) {
	if (list == NULL || element == NULL) {
		return EINVAL;
	}

	struct list_node *np = NULL; // node pointer
	if (list->head == NULL) {
		// empty list
		return EADDRNOTAVAIL;
	}

	np = list->head;
	if (list->head->element == element) {
		list->head = np->next;
		if (list->head == NULL) {
			list->tail = NULL;
		} else {
			list->head->previous = NULL;
		}
	} else {
		while (np != NULL && np->element != element) {
			np = np->next;
		}

		if (np == NULL) {
			//element not in list
			return EADDRNOTAVAIL;
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


void *list_shift(struct list *list) {
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

	void *element = np->element;
	list_delnode(list, np);

	return element;
}


void *list_pop(struct list *list) {
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

	void *element = np->element;
	list_delnode(list, np);

	return element;
}


int list_size(struct list *list) {
	return list == NULL ? -EINVAL : list->nelement;
}


void TestListErroneousInput(CuTest *tc) {
	int somenumber;

	struct list *list = list_init();
	CuAssertPtrNotNull(tc, list);

	CuAssertIntEquals(tc, EINVAL, list_prepend(NULL, &somenumber));
	CuAssertIntEquals(tc, EINVAL, list_prepend(list, NULL));
	CuAssertIntEquals(tc, EINVAL, list_append(NULL, &somenumber));
	CuAssertIntEquals(tc, EINVAL, list_append(list, NULL));
	CuAssertIntEquals(tc, EINVAL, list_remove(NULL, &somenumber));
	CuAssertIntEquals(tc, EINVAL, list_remove(list, NULL));
	CuAssertIntEquals(tc, -EINVAL, list_size(NULL));
	CuAssertPtrEquals(tc, NULL, list_shift(NULL));
	CuAssertPtrEquals(tc, NULL, list_pop(NULL));
	list_free(NULL);
}


void TestListEmpty(CuTest *tc) {
	struct list *list = list_init();
	CuAssertPtrNotNull(tc, list);

	int somenumber;
	CuAssertIntEquals(tc, EADDRNOTAVAIL, list_remove(list, &somenumber));

	CuAssertPtrEquals(tc, NULL, list_shift(list));
	CuAssertPtrEquals(tc, NULL, list_pop(list));
	list_free(list);
}


void TestListNonempty(CuTest *tc) {
	enum {
		NELEMENT = 100,
	};
	int elements[NELEMENT];

	struct list *list = list_init();
	CuAssertPtrNotNull(tc, list);

	int i;
	for (i = 0; i < NELEMENT; i++) {
		elements[i] = NELEMENT-i;

		CuAssertIntEquals(tc, i, list_size(list));
		CuAssertIntEquals(tc, 0, list_append(list, elements+i));
		CuAssertIntEquals(tc, i+1, list_size(list));
	}

	int somenumber;
	CuAssertIntEquals(tc, EADDRNOTAVAIL, list_remove(list, &somenumber));

	for (i = NELEMENT; i > 0; i--) {
		CuAssertIntEquals(tc, i, list_size(list));
		CuAssertIntEquals(tc, 0, list_remove(list, elements+(i-1)));
		CuAssertIntEquals(tc, i-1, list_size(list));
	}

	for (i = 0; i < NELEMENT; i++) {
		CuAssertIntEquals(tc, i ,list_size(list));
		CuAssertIntEquals(tc, 0, list_append(list, elements+i));
		CuAssertIntEquals(tc, i+1 ,list_size(list));
	}

	for (i = 0; i < NELEMENT>>1; i++) {
		CuAssertIntEquals(tc, NELEMENT-i, list_size(list));
		CuAssertPtrEquals(tc, elements+i, list_shift(list));
		CuAssertIntEquals(tc, NELEMENT-i-1, list_size(list));
	}

	for (i = NELEMENT>>1; i > 0; i--) {
		CuAssertIntEquals(tc, NELEMENT-i, list_size(list));
		CuAssertIntEquals(tc, 0, list_prepend(list, elements+(i-1)));
		CuAssertIntEquals(tc, NELEMENT-i+1, list_size(list));
	}

	for (i = NELEMENT; i > 0; i--) {
		CuAssertIntEquals(tc, i, list_size(list));
		CuAssertPtrEquals(tc, elements+(i-1), list_pop(list));
		CuAssertIntEquals(tc, i-1, list_size(list));
	}


	list_free(list);
}



/* Double linked list iterator implementation */


struct list_iterator {
	struct list_node  *next, *previous;
	struct list       *list;
};


struct list_iterator *list_iterator_init(struct list *list) {
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


void list_iterator_free(struct list_iterator *li) {
	free(li);
}


void *list_iterator_next(struct list_iterator *li) {
	if (li == NULL) {
		return NULL;
	}

	if (li->next == NULL) {
		return NULL;
	}

	void *element = li->next->element;

	li->previous = li->next;
	li->next = li->next->next;

	return element;
}


void *list_iterator_previous(struct list_iterator *li) {
	if (li == NULL) {
		return NULL;
	}

	if (li->previous == NULL) {
		return NULL;
	}

	void *element = li->previous->element;

	li->next = li->previous;
	li->previous = li->previous->previous;

	return element;
}



int list_iterator_reset(struct list_iterator *li) {
	if (li == NULL) {
		return EINVAL;
	}

	li->next = li->list->head;
	li->previous = li->list->tail;
	return 0;
}


void TestList_iteratorErroneousInput(CuTest *tc) {
	CuAssertPtrEquals(tc, NULL, list_iterator_init(NULL));
	CuAssertPtrEquals(tc, NULL, list_iterator_next(NULL));
	CuAssertPtrEquals(tc, NULL, list_iterator_previous(NULL));
	CuAssertIntEquals(tc, EINVAL, list_iterator_reset(NULL));
	list_iterator_free(NULL);
}


void TestList_iteratorEmpty(CuTest *tc) {
	struct list *list = list_init();
	CuAssertPtrNotNull(tc, list);
	struct list_iterator *li = list_iterator_init(list);
	CuAssertPtrNotNull(tc, li);

	CuAssertPtrEquals(tc, NULL, list_iterator_next(li));
	CuAssertPtrEquals(tc, NULL, list_iterator_previous(li));
	CuAssertIntEquals(tc, 0, list_iterator_reset(li));

	list_iterator_free(li);
	list_free(list);
}


void TestList_iteratorNonempty(CuTest *tc) {
	enum {
		NELEMENT = 100,
	};
	int elements[NELEMENT];

	struct list *list = list_init();
	CuAssertPtrNotNull(tc, list);
	struct list_iterator *li = list_iterator_init(list);
	CuAssertPtrNotNull(tc, li);

	int i;
	for (i = 0; i < NELEMENT; i++) {
		elements[i] = NELEMENT-i;
		list_append(list, elements+i);
	}

	CuAssertIntEquals(tc, 0, list_iterator_reset(li));

	for (i = 0; i < NELEMENT; i++) {
		CuAssertPtrEquals(tc, elements+i, list_iterator_next(li));
	}
	CuAssertPtrEquals(tc, NULL, list_iterator_next(li));

	for (i = NELEMENT-1; i >= 0; i--) {
		CuAssertPtrEquals(tc, elements+i, list_iterator_previous(li));
	}
	CuAssertPtrEquals(tc, NULL, list_iterator_previous(li));


	CuAssertIntEquals(tc, 0, list_iterator_reset(li));
	CuAssertPtrEquals(tc, elements, list_iterator_next(li));
	CuAssertIntEquals(tc, 0, list_iterator_reset(li));
	CuAssertPtrEquals(tc, elements+(NELEMENT-1), list_iterator_previous(li));

	list_iterator_free(li);
	list_free(list);
}
