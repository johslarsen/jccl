#ifndef STACK_H
#define STACH_H

/* Stack interface */
typedef struct Stack Stack;

/* Create new stack
 * NULL on error */
Stack *stack_init(long maxsize);

/* Free the memory taken up by the stack 
 * The data that items points to are preserved
 * value != 0 en error */
int stack_free(Stack *stack);

/* Insert item at top of stack. O=1
 * value != 0 on error */
int stack_push(Stack *stack, void *item);

/* Pop the item at the top of stack. O=1
 * NULL on error */
void *stack_pop(Stack *stack);

/* Return number of items in the list. O=1
 * value < 0 on error */
long stack_size(Stack *stack);

#endif /*STACK_H*/
