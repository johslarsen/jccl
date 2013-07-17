#ifndef TREE_H
#define TREE_H

/*
 * comparator function
 *
 * returns:
 *   <0 <-- a < b
 *   0 <-- a == b
 *   >0 <-- a > b
 */
typedef int (*tree_comparator)(void const *a, void const *b);

/*
 * tree initializer.
 *
 * compar:
 *
 * returns:
 *   error --> NULL
 *   --> *(new tree)
 */
extern struct tree *tree_init(tree_comparator compar);

/*
 * tree destructor. if tree == NULL it does nothing.
 */
extern void tree_destroy(struct tree *tree);

/*
 * number of elements in the tree
 *
 * returns:
 *   tree == NULL --> -EINVAL
 *   --> number of elements
 */
extern int tree_size(struct tree *tree);

/*
 * the maximum depth of the tree.
 *
 * returns:
 *   tree == NULL --> -EINVAL
 *   --> the maximum depth
 */
extern int tree_depth(struct tree *tree);

/*
 * add a key value pair to the tree.
 * behavior is undefined if key is already in the tree
 *
 * returns:
 *   tree == NULL || key == NULL --> EINVAL
 *   failure to create node --> errno
 *   --> 0
 */
extern int tree_add(struct tree *tree, void const *key, void const *value);

/*
 * find the value to the first node such that compar(key, node->key) == 0.
 *
 * returns:
 *   tree == NULL || key == NULL --> NULL
 *   key not in tree --> NULL
 *   --> head->value, if index_of_key != NULL, it contains the index in the tree
 */
extern void *tree_search(struct tree *tree, const void *key, int *index_of_key);

/*
 * removes the first key-value pair where compar(key, node->key) == 0
 *
 * returns:
 *   tree == NULL || key == NULL --> EINVAL
 *   key not in tree --> ESRCH
 *   --> 0
 */
extern int tree_remove(struct tree *tree, void const *key);

/*
 * retrieve the i-th key-value pair in the tree.
 * negative i counts backwards from end of tree.
 *
 * returns:
 *   tree == NULL || key == NULL || value == NULL --> EINVAL
 *   |i| >= tree_size(tree) --> ESRCH
 *   --> 0, *key contains node->key, *value contains node->value
 */
extern int tree_select(struct tree *tree, int i, void **key, void **value);

/*
 * called on a key-value pair in the tree with key = node->key, value =
 * node->value and state is passed along from caller.
 *
 * returns:
 *   0 <-- success
 *   !0 <-- failure/abort
 */
typedef int (*tree_action)(const void *key, const void *value, void *state);

/*
 * traverse the key-value pairs of the tree in-order. performs the action on
 * each key-value as long as its succeeds, i.e. break on the first actions that
 * fails.
 *
 * returns:
 *   tree == NULL || action == NULL --> EINVAL
 *   --> return value of last action
 */
extern int tree_for_each(struct tree *tree, tree_action action, void *state);


#endif /*TREE_H*/
