#ifndef dict_H
#define dict_H

/*
 * comparator function
 *
 * returns:
 *   <0 <-- a < b
 *   0 <-- a == b
 *   >0 <-- a > b
 */
typedef int (*dict_comparator)(void const *a, void const *b);

/*
 * dict initializer.
 *
 * compar:
 *
 * returns:
 *   error --> NULL
 *   --> *(new dict)
 */
extern struct dict *dict_init(dict_comparator compar);

/*
 * dict destructor. if dict == NULL it does nothing.
 */
extern void dict_destroy(struct dict *dict);

/*
 * number of elements in the dict
 *
 * returns:
 *   dict == NULL --> -EINVAL
 *   --> number of elements
 */
extern int dict_size(struct dict *dict);

/*
 * maps the key to the value.
 *
 * returns:
 *   dict == NULL || key == NULL --> EINVAL
 *   failure to create node --> errno
 *   --> 0,
 *     nkey != NULL -> *nkey = node ? node->key : NULL
 *     nvalue != NULL -> *nvalue = node ? node->value : NULL
 */
extern int dict_put(struct dict *dict, void const *key, void const *value, void **node, void **nvalue);

/*
 * find the value to the node such that compar(key, node->key) == 0.
 *
 * returns:
 *   dict == NULL || key == NULL --> NULL
 *   key not in dict --> NULL
 *   --> node->value
 *     index_of_key != NULL --> *index_of_key contains the index in the dict
 */
extern void *dict_get(struct dict *dict, const void *key, int *index_of_key);

/*
 * removes the key-value pair such that compar(key, node->key) == 0
 *
 * NOTE: the contents of node->key,node->value is untouched and NOT freed
 *
 * returns:
 *   dict == NULL || key == NULL --> EINVAL
 *   key not in dict --> ESRCH
 *   --> 0
 *     nkey != NULL -> *nkey = node->key
 *     nvalue != NULL -> *nvalue = node->value
 */
extern int dict_remove(struct dict *dict, void const *key, void **nkey, void **nvalue);

/*
 * retrieve the i-th key-value pair in the dict.
 * negative i counts backwards from end of dict.
 *
 * returns:
 *   dict == NULL || key == NULL || value == NULL --> EINVAL
 *   |i| >= dict_size(dict) --> ESRCH
 *   --> 0
 *     *nkey = node->key
 *     *nvalue = node->value
 */
extern int dict_select(struct dict *dict, int i, void **key, void **value);

/*
 * called on a key-value pair in the dict with key = node->key, value =
 * node->value and state is passed along from caller.
 *
 * returns:
 *   0 <-- success
 *   !0 <-- failure/abort
 */
typedef int (*dict_action)(const void *key, const void *value, void *state);

/*
 * traverse the key-value pairs of the dict in-order. performs the action on
 * each key-value as long as its succeeds, i.e. break on the first actions that
 * fails.
 *
 * returns:
 *   dict == NULL || action == NULL --> EINVAL
 *   --> return value of last action
 */
extern int dict_for_each(struct dict *dict, dict_action action, void *state);


#endif /*dict_H*/
