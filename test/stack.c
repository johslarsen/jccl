#include <errno.h>
#include <stdlib.h>
#include <stdio.h>

#include "../stack.h"
#include "../unittest.h"

enum {
	NITEM = 1024, // items to test with
};

void stack_common_test(Stack *stack, int limited)
{
	static int items[NITEM+1]; // testing 1 past limit

	int i;
	for (i = 0; i < NITEM+1; i++) {
		items[i] = rand();
	}

	// empty list
	UNITTEST(stack_pop(stack) == NULL);
	UNITTEST(stack_size(stack) == 0);
		
	for (i = 0; i < NITEM; i++) {
		UNITTEST(stack_push(stack, &items[i]) == 0);
		UNITTEST(stack_size(stack) == i+1);
	}

	if (limited == 1) {
		UNITTEST(stack_push(stack, &items[i--]) == EPERM);
		UNITTEST(stack_size(stack) == NITEM);
	} else {
		UNITTEST(stack_push(stack, &items[i]) == 0);
		UNITTEST(stack_size(stack) == NITEM+1);
		UNITTEST(stack_pop(stack) == &items[i--]);
		UNITTEST(stack_size(stack) == NITEM);
	}

	do {
		UNITTEST(stack_pop(stack) == &items[i--]);
		UNITTEST(stack_size(stack) == i+1);
	} while (i > 0);

	UNITTEST(stack_free(stack)==0);
}

void stack_test(void)
{
	// testing return values when list is NULL
	int someitem = rand();
	UNITTEST(stack_free(NULL) == EINVAL);
	UNITTEST(stack_push(NULL, &someitem) == EINVAL);
	UNITTEST(stack_pop(NULL) == NULL);
	UNITTEST(stack_size(NULL) == -EINVAL);

	Stack *stack = stack_init(0);
	if (stack == NULL) {
		fprintf(stderr, "stack_test: can not create infinite stack, aborting this stack\n");
	} else {
		stack_common_test(stack, 0);
	}

	stack = stack_init(NITEM);
	if (stack == NULL) {
		fprintf(stderr, "stack_test: can not create the finite stack, aborting this stack\n");
	} else {
		stack_common_test(stack, 1);
	}
}

int main(void)
{
	stack_test();
	return 0;
}
