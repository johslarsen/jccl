#include "../list.h"
#include "../unittest.h"
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>


enum {
	NITEM = 100,
};


void list_test(void)
{
	int somenumber = rand();
	void *item = &somenumber;

	UNITTEST(list_destroy(NULL) == EINVAL);
	UNITTEST(list_prepend(NULL, item) == EINVAL);
	UNITTEST(list_append(NULL, item) == EINVAL);
	UNITTEST(list_remove(NULL, item) == EINVAL);
	UNITTEST(list_pop(NULL) == NULL);
	UNITTEST(list_size(NULL) == -EINVAL);

	List *list = list_create();
	if (list == NULL) {
		fprintf(stderr, "list_test: can not create list, aborting\n");
		return;
	}

	int items[NITEM];
	int i;
	for (i = 0; i < NITEM; i++) {
		items[i] = rand();
		UNITTEST(list_size(list) == i);
		UNITTEST(list_append(list, &items[i]) == 0);
	}
	UNITTEST(list_size(list) == i);

	for (i = NITEM-1; i >= 0; i--) {
		UNITTEST(list_remove(list, &items[i]) == 0);
		UNITTEST(list_size(list) == i);
	}

	for (i = 0; i < NITEM; i++) {
		UNITTEST(list_size(list) == i);
		UNITTEST(list_prepend(list, &items[i]) == 0);
	}
	UNITTEST(list_size(list) == i);

	for (i = 0; i < NITEM; i++) {
		UNITTEST(list_size(list) == NITEM-i);
		UNITTEST(list_pop(list) == &items[NITEM-1-i]);
	}

	UNITTEST(list_destroy(list) == 0);
}


void list_iterator_test(void)
{
	UNITTEST(list_createiterator(NULL) == NULL);
	UNITTEST(list_next(NULL) == NULL);
	UNITTEST(list_resetiterator(NULL) == EINVAL);
	UNITTEST(list_destroyiterator(NULL) == EINVAL);

	List *list = list_create();
	if (list == NULL) {
		fprintf(stderr, "list_iterator_test: can not create list, aborting\n");
		return;
	}
	List_iterator *li = list_createiterator(list);
	if (list == NULL) {
		fprintf(stderr, "list_iterator_test: can not create list_iterator, aborting\n");
		return;
	}

	int items[NITEM];
	int i;
	for (i = 0; i < NITEM; i++) {
		items[i] = rand();
		list_append(list, &items[i]);
		UNITTEST(list_next(li) == NULL); // should point to the head of the list as of its creation, empty list => NULL
	}

	UNITTEST(list_resetiterator(li) == 0);
	for (i = 0; i < NITEM; i++) {
		UNITTEST(list_next(li) == &items[i]);
		UNITTEST(list_size(list) == NITEM);
	}
	UNITTEST(list_next(li) == NULL);

	UNITTEST(list_resetiterator(li) == 0);
	for(i = 0; i < NITEM/2; i++) {
		UNITTEST(list_next(li) == &items[i]);
	}

	UNITTEST(list_destroyiterator(li) == 0);
	list_destroy(list);
}


int main(void)
{
	list_test();
	list_iterator_test();
	return 0;
}
