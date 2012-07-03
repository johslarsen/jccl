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

	UNITTEST(list_prepend(NULL, item) == EINVAL);
	UNITTEST(list_append(NULL, item) == EINVAL);
	UNITTEST(list_remove(NULL, item) == EINVAL);
	UNITTEST(list_shift(NULL) == NULL);
	UNITTEST(list_pop(NULL) == NULL);
	UNITTEST(list_size(NULL) == -EINVAL);

	struct list *list = list_init();
	if (list == NULL) {
		fprintf(stderr, "list_test: can not create list, aborting\n");
		return;
	}

	// empty list
	UNITTEST(list_remove(list, item) == EINVAL);
	UNITTEST(list_shift(list) == NULL);
	UNITTEST(list_pop(list) == NULL);

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
		UNITTEST(list_shift(list) == &items[NITEM-1-i]); // items were added in reverse order
	}
	UNITTEST(list_size(list) == 0);

	for (i = 0; i < NITEM; i++) {
		UNITTEST(list_append(list, &items[i]) == 0);
	}

	for (i = 0; i < NITEM; i++) {
		UNITTEST(list_size(list) == NITEM-i);
		UNITTEST(list_pop(list) == &items[NITEM-1-i]);
	}
	UNITTEST(list_size(list) == 0);

	list_free(list);
}


void list_iterator_test(void)
{
	UNITTEST(list_iterator_init(NULL) == NULL);
	UNITTEST(list_iterator_next(NULL) == NULL);
	UNITTEST(list_iterator_reset(NULL) == EINVAL);

	struct list *list = list_init();
	if (list == NULL) {
		fprintf(stderr, "list_iterator_test: can not create list, aborting\n");
		return;
	}
	struct list_iterator *li = list_iterator_init(list);
	if (list == NULL) {
		fprintf(stderr, "list_iterator_test: can not create list_iterator, aborting\n");
		return;
	}

	// empty list
	UNITTEST(list_iterator_next(li) == NULL);
	UNITTEST(list_iterator_previous(li) == NULL);

	int items[NITEM];
	int i;
	for (i = 0; i < NITEM; i++) {
		items[i] = rand();
		list_append(list, &items[i]);
	}

	UNITTEST(list_iterator_reset(li) == 0);

	// forward
	for (i = 0; i < NITEM; i++) {
		UNITTEST(list_iterator_next(li) == &items[i]);
	}
	UNITTEST(list_iterator_next(li) == NULL);

	// backward
	for (i-= 1; i >= 0; i--) {
		UNITTEST(list_iterator_previous(li) == &items[i]);
	}
	UNITTEST(list_iterator_previous(li) == NULL);

	UNITTEST(list_iterator_reset(li) == 0);
	UNITTEST(list_iterator_next(li) == &items[0]);

	UNITTEST(list_iterator_reset(li) == 0);
	UNITTEST(list_iterator_previous(li) == &items[NITEM-1]);

	list_iterator_free(li);
	list_free(list);
}


int main(void)
{
	list_test();
	list_iterator_test();
	return 0;
}
