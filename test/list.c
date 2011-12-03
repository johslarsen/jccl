#include "../list.h"
#include <assert.h>
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

	assert(list_destroy(NULL) == EINVAL);
	assert(list_prepend(NULL, item) == EINVAL);
	assert(list_append(NULL, item) == EINVAL);
	assert(list_remove(NULL, item) == EINVAL);
	assert(list_pop(NULL) == NULL);
	assert(list_size(NULL) == -EINVAL);

	List *list = list_create();
	assert(list != NULL);

	int items[NITEM];
	int i;
	for (i = 0; i < NITEM; i++) {
		items[i] = rand();
		assert(list_size(list) == i);
		assert(list_append(list, &items[i]) == 0);
	}
	assert(list_size(list) == i);

	for (i = NITEM-1; i >= 0; i--) {
		assert(list_remove(list, &items[i]) == 0);
		assert(list_size(list) == i);
	}

	for (i = 0; i < NITEM; i++) {
		assert(list_size(list) == i);
		assert(list_prepend(list, &items[i]) == 0);
	}
	assert(list_size(list) == i);

	for (i = 0; i < NITEM; i++) {
		assert(list_size(list) == NITEM-i);
		assert(list_pop(list) == &items[NITEM-1-i]);
	}

	assert(list_destroy(list) == 0);
}


void list_iterator_test(void)
{
	assert(list_createiterator(NULL) == NULL);
	assert(list_next(NULL) == NULL);
	assert(list_resetiterator(NULL) == EINVAL);
	assert(list_destroyiterator(NULL) == EINVAL);

	List *list = list_create();
	List_iterator *li = list_createiterator(list);
	assert(li != NULL);

	int items[NITEM];
	int i;
	for (i = 0; i < NITEM; i++) {
		items[i] = rand();
		list_append(list, &items[i]);
		assert(list_next(li) == NULL); // should point to the head of the list as of its creation, empty list => NULL
	}

	assert(list_resetiterator(li) == 0);
	for (i = 0; i < NITEM; i++) {
		assert(list_next(li) == &items[i]);
		assert(list_size(list) == NITEM);
	}
	assert(list_next(li) == NULL);

	assert(list_resetiterator(li) == 0);
	for(i = 0; i < NITEM/2; i++) {
		assert(list_next(li) == &items[i]);
	}

	assert(list_destroyiterator(li) == 0);
	list_destroy(list);
}


int main(void)
{
	list_test();
	list_iterator_test();
	return 0;
}
