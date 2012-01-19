#ifndef STACK_H
#define STACH_H

/* Stack interface */
typedef struct Stack Stack;

/* Create new stack. ORDER=1
 * return NULL on error */
Stack *stack_init(long maxsize);

/* Free the memory taken up by the stack. ORDER=1
 * return != 0 en error */
int stack_free(Stack *stack);

/* Insert item at top of stack. ORDER=1
 * return != 0 on error, EPERM on NITEM > maxsize */
int stack_push(Stack *stack, void *item);

/* Pop the item at the top of stack. ORDER=1
 * return NULL on error or empty stack */
void *stack_pop(Stack *stack);

/* Return number of items in the list. ORDER=1
 * return < 0 on error */
long stack_size(Stack *stack);

#endif /*STACK_H*/
