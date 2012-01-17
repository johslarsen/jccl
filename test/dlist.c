#include "../dlist.h"
#include "../unittest.h"
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>


enum {
	NITEM = 100,
};


void dlist_test(void)
{
	int somenumber = rand();
	void *item = &somenumber;

	UNITTEST(dlist_free(NULL) == EINVAL);
	UNITTEST(dlist_prepend(NULL, item) == EINVAL);
	UNITTEST(dlist_append(NULL, item) == EINVAL);
	UNITTEST(dlist_remove(NULL, item) == EINVAL);
	UNITTEST(dlist_pop(NULL) == NULL);
	UNITTEST(dlist_size(NULL) == -EINVAL);

	Dlist *list = dlist_init();
	if (list == NULL) {
		fprintf(stderr, "dlist_test: can not create list, aborting\n");
		return;
	}

	int items[NITEM];
	int i;
	for (i = 0; i < NITEM; i++) {
		items[i] = rand();
		UNITTEST(dlist_size(list) == i);
		UNITTEST(dlist_append(list, &items[i]) == 0);
	}
	UNITTEST(dlist_size(list) == i);

	for (i = NITEM-1; i >= 0; i--) {
		UNITTEST(dlist_remove(list, &items[i]) == 0);
		UNITTEST(dlist_size(list) == i);
	}

	for (i = 0; i < NITEM; i++) {
		UNITTEST(dlist_size(list) == i);
		UNITTEST(dlist_prepend(list, &items[i]) == 0);
	}
	UNITTEST(dlist_size(list) == i);

	for (i = 0; i < NITEM; i++) {
		UNITTEST(dlist_size(list) == NITEM-i);
		UNITTEST(dlist_pop(list) == &items[NITEM-1-i]);
	}

	UNITTEST(dlist_free(list) == 0);
}


void dlist_iterator_test(void)
{
	UNITTEST(dlist_iterator_init(NULL) == NULL);
	UNITTEST(dlist_iterator_next(NULL) == NULL);
	UNITTEST(dlist_iterator_reset(NULL) == EINVAL);
	UNITTEST(dlist_iterator_free(NULL) == EINVAL);

	Dlist *list = dlist_init();
	if (list == NULL) {
		fprintf(stderr, "dlist_iterator_test: can not create list, aborting\n");
		return;
	}
	Dlist_iterator *li = dlist_iterator_init(list);
	if (list == NULL) {
		fprintf(stderr, "dlist_iterator_test: can not create dlist_iterator, aborting\n");
		return;
	}

	int items[NITEM];
	int i;
	for (i = 0; i < NITEM; i++) {
		items[i] = rand();
		dlist_append(list, &items[i]);
		UNITTEST(dlist_iterator_next(li) == NULL); // should point to the head of the list as of its creation, empty list => NULL
	}

	UNITTEST(dlist_iterator_reset(li) == 0);
	for (i = 0; i < NITEM; i++) {
		UNITTEST(dlist_iterator_next(li) == &items[i]);
		UNITTEST(dlist_size(list) == NITEM);
	}
	UNITTEST(dlist_iterator_next(li) == NULL);

	UNITTEST(dlist_iterator_reset(li) == 0);
	for(i = 0; i < NITEM/2; i++) {
		UNITTEST(dlist_iterator_next(li) == &items[i]);
	}

	UNITTEST(dlist_iterator_free(li) == 0);
	dlist_free(list);
}


int main(void)
{
	dlist_test();
	dlist_iterator_test();
	return 0;
}
