#include <errno.h>
#include <stdlib.h>
#include <stdio.h>

#include "stack.h"

/* Stack implementation */

enum {
	STACK_INITIAL_SIZE = 16, // initial size of stack
};

struct Stack {
	long	maxsize;
	long	cursize;
	long	nitem;
	void	**items;
};

/* Increase the size of the the stack by a multiple of 2 
 * using realloc so that the items are preserved */
static int stack_expand(Stack *stack)
{
	if (stack == NULL) {
		return EINVAL;
	}

	if (stack->cursize == 0) {
		stack->cursize = STACK_INITIAL_SIZE;
	} else {
		stack->cursize <<= 1;
	}

	void **newitems = (void **)realloc(stack->items, stack->cursize*sizeof(*stack->items));
	if (newitems == NULL) {
		stack->cursize >>= 1;
		return errno;
	} else {
		stack->items = newitems;
	}

	return 0;
}

Stack *stack_init(long maxsize)
{
	Stack *stack = (Stack *)calloc(1, sizeof(*stack));
	if (stack == NULL) {
		goto error;
	}

	stack->maxsize = maxsize;
	stack_expand(stack);
	if (stack->items == NULL) {
		goto error;
	}

	return stack;
error:
	stack_free(stack);
	return NULL;
}


int stack_free(Stack *stack)
{
	if (stack == NULL) {
		return EINVAL;
	}

	free(stack->items);
	free(stack);

	return 0;
}	


int stack_push(Stack *stack, void *item)
{
	if (stack == NULL) {
		return EINVAL;
	}

	if (stack->maxsize > 0 && stack->nitem >= stack->maxsize) {
		return EPERM;
	}

	while (stack->nitem >= stack->cursize) {
		int retval = stack_expand(stack);
		if (retval != 0) {
			return retval;
		}
	}

	stack->items[stack->nitem++] = item;
	return 0;
}


void *stack_pop(Stack *stack)
{
	if (stack == NULL) {
		goto error;
	}

	if (stack->nitem > 0) {
		return stack->items[--stack->nitem];
	} else {
		goto error;
	}
error:
	return NULL;
}


long stack_size(Stack *stack)
{
	if (stack == NULL) {
		return -EINVAL;
	}

	return stack->nitem;
}
