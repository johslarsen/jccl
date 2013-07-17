#include "CuTest/CuTest.h"
#include "tree.h"
#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

/* Self-balancing binary search tree
 *
 * based on aatree (Arne Andersson tree), http://user.it.uu.se/~arnea/ps/simp.pdf */

struct subtree {
	struct subtree *left, *right;
	const void *key, *value;
	int nnode, level;
};

struct tree {
	struct subtree *head;
	tree_comparator compar;
};

struct subtree end_of_tree_sentinel = {.left = &end_of_tree_sentinel, .right = &end_of_tree_sentinel};
#define EOT(node) ((struct subtree *)(node) == (&end_of_tree_sentinel))


static int compare_pointers(const void *a, const void *b)
{
	return (int)(a-b);
}


static struct subtree *node_init(const void *key, const void *value)
{
	struct subtree *node = malloc(sizeof(*node));
	if (node == NULL) {
		return NULL;
	}

	node->key = key;
	node->value = value;
	node->left = &end_of_tree_sentinel;
	node->right = &end_of_tree_sentinel;
	node->nnode = 1;
	node->level = 1;

	return node;
}


static void node_destroy(struct subtree *node)
{
	assert(node != NULL);
	// NOTE: assuming children are destroyed

	if (!EOT(node)) {
		free(node);
	}
}


void TestNodeConstructionAndDestruction(CuTest *tc)
{
	int key = 0xdeadbeef;
	int value = 0xc0ffee;

	struct subtree *node = node_init(&key, &value);
	CuAssertPtrNotNull(tc, node);
	CuAssertPtrEquals(tc, &key, (int *)node->key);
	CuAssertPtrEquals(tc, &value, (int *)node->value);
	CuAssertIntEquals(tc, 1, node->nnode);
	CuAssertIntEquals(tc, 1, node->level);
	CuAssertTrue(tc, EOT(node->left));
	CuAssertTrue(tc, EOT(node->right));

	node_destroy(node);
}


struct tree *tree_init(tree_comparator compar)
{
	if (compar == NULL) {
		return NULL;
	}

	struct tree *tree = malloc(sizeof(*tree));
	if (tree == NULL) {
		return NULL;
	}

	tree->head = &end_of_tree_sentinel;
	tree->compar = compar;

	return tree;
}


static void subtree_destroy(struct subtree *head)
{
	assert(head != NULL);
	if (EOT(head)) {
		return;
	}

	subtree_destroy(head->left);
	subtree_destroy(head->right);

	node_destroy(head);
}

void tree_destroy(struct tree *tree)
{
	if (tree != NULL) {
		subtree_destroy(tree->head);
		free(tree);
	}
}


static int subtree_size(struct subtree *head)
{
	assert(head != NULL);
	return head->nnode;
}

int tree_size(struct tree *tree)
{
	return tree == NULL ? -EINVAL : subtree_size(tree->head);
}


void TestTreeConstructionAndDestruction(CuTest *tc)
{
	struct tree *tree = tree_init(compare_pointers);
	CuAssertPtrNotNull(tc, tree);

	CuAssertIntEquals(tc, 0, tree_size(tree));

	tree_destroy(tree);
}


enum {
	NNODE_1_LAYER_BALANCED_TREE = 1,
	NNODE_2_LAYER_BALANCED_TREE = 3,
	NNODE_3_LAYER_BALANCED_TREE = 7,
};
static struct subtree *dummy_1_layer_balanced_tree(struct subtree *a_node)
{
	assert(a_node != NULL && !EOT(a_node));

	struct subtree *first   = a_node+0;      first->key   = (void *)1; first->nnode   = 1; first->left   = &end_of_tree_sentinel; first->right   = &end_of_tree_sentinel;

	return first;
}
static struct subtree *dummy_2_layer_balanced_tree(struct subtree *three_nodes)
{
	assert(three_nodes != NULL && !EOT(three_nodes));

	struct subtree *first   = three_nodes+0; first->key   = (void *)1; first->nnode   = 1; first->left   = &end_of_tree_sentinel; first->right   = &end_of_tree_sentinel;
	struct subtree *third   = three_nodes+1; third->key   = (void *)3; third->nnode   = 1; third->left   = &end_of_tree_sentinel; third->right   = &end_of_tree_sentinel;
	struct subtree *second  = three_nodes+4; second->key  = (void *)2; second->nnode  = 3; second->left  = first;                 second->right  = third;

	return second;
}
static struct subtree *dummy_3_layer_balanced_tree(struct subtree *seven_nodes)
{
	assert(seven_nodes != NULL && !EOT(seven_nodes));

	struct subtree *first   = seven_nodes+0; first->key   = (void *)1; first->nnode   = 1; first->left   = &end_of_tree_sentinel; first->right   = &end_of_tree_sentinel;
	struct subtree *third   = seven_nodes+1; third->key   = (void *)3; third->nnode   = 1; third->left   = &end_of_tree_sentinel; third->right   = &end_of_tree_sentinel;
	struct subtree *fifth   = seven_nodes+2; fifth->key   = (void *)5; fifth->nnode   = 1; fifth->left   = &end_of_tree_sentinel; fifth->right   = &end_of_tree_sentinel;
	struct subtree *seventh = seven_nodes+3; seventh->key = (void *)7; seventh->nnode = 1; seventh->left = &end_of_tree_sentinel; seventh->right = &end_of_tree_sentinel;
	struct subtree *second  = seven_nodes+4; second->key  = (void *)2; second->nnode  = 3; second->left  = first;                 second->right  = third;
	struct subtree *sixth   = seven_nodes+5; sixth->key   = (void *)6; sixth->nnode   = 3; sixth->left   = fifth;                 sixth->right   = seventh;
	struct subtree *fourth  = seven_nodes+6; fourth->key  = (void *)4; fourth->nnode  = 7; fourth->left  = second;                 fourth->right  = sixth;

	return fourth;
}
void TestTreeDummyBalancedTrees(CuTest *tc)
{
	struct subtree nodes[7];

	struct subtree *head = dummy_1_layer_balanced_tree(nodes);
	CuAssertPtrNotNull(tc, head);
	CuAssertIntEquals(tc, NNODE_1_LAYER_BALANCED_TREE, subtree_size(head));
	CuAssertPtrEquals(tc, (void const *)1, head->key);
	CuAssertIntEquals(tc, NNODE_1_LAYER_BALANCED_TREE, head->nnode);

	head = dummy_2_layer_balanced_tree(nodes);
	CuAssertPtrNotNull(tc, head);
	CuAssertIntEquals(tc, NNODE_2_LAYER_BALANCED_TREE, subtree_size(head));
	CuAssertPtrEquals(tc, (void const *)1, head->left->key);
	CuAssertPtrEquals(tc, (void const *)2, head->key);
	CuAssertPtrEquals(tc, (void const *)3, head->right->key);
	CuAssertIntEquals(tc, NNODE_1_LAYER_BALANCED_TREE, head->left->nnode);
	CuAssertIntEquals(tc, NNODE_2_LAYER_BALANCED_TREE, head->nnode);
	CuAssertIntEquals(tc, NNODE_1_LAYER_BALANCED_TREE, head->right->nnode);

	head = dummy_3_layer_balanced_tree(nodes);
	CuAssertPtrNotNull(tc, head);
	CuAssertIntEquals(tc, NNODE_3_LAYER_BALANCED_TREE, subtree_size(head));
	CuAssertPtrEquals(tc, (void const *)1, head->left->left->key);
	CuAssertPtrEquals(tc, (void const *)2, head->left->key);
	CuAssertPtrEquals(tc, (void const *)3, head->left->right->key);
	CuAssertPtrEquals(tc, (void const *)4, head->key);
	CuAssertPtrEquals(tc, (void const *)5, head->right->left->key);
	CuAssertPtrEquals(tc, (void const *)6, head->right->key);
	CuAssertPtrEquals(tc, (void const *)7, head->right->right->key);
	CuAssertIntEquals(tc, NNODE_1_LAYER_BALANCED_TREE, head->left->left->nnode);
	CuAssertIntEquals(tc, NNODE_2_LAYER_BALANCED_TREE, head->left->nnode);
	CuAssertIntEquals(tc, NNODE_1_LAYER_BALANCED_TREE, head->left->right->nnode);
	CuAssertIntEquals(tc, NNODE_3_LAYER_BALANCED_TREE, head->nnode);
	CuAssertIntEquals(tc, NNODE_1_LAYER_BALANCED_TREE, head->right->left->nnode);
	CuAssertIntEquals(tc, NNODE_2_LAYER_BALANCED_TREE, head->right->nnode);
	CuAssertIntEquals(tc, NNODE_1_LAYER_BALANCED_TREE, head->right->right->nnode);
}


#if 0
struct node_position {
	struct subtree *node;
	int depth;
};
static void subtree_fill_positions(struct subtree *head, struct node_position *lines, int depth, int nnode_parent)
{
	assert(head != NULL && lines != NULL);
	if (EOT(head)) {
		return;
	}

	subtree_fill_positions(head->left, lines, depth+1, nnode_parent);
	struct node_position *line = lines + nnode_parent + head->left->nnode;
	line->node = head;
	line->depth = depth;
	subtree_fill_positions(head->right, lines, depth+1, nnode_parent + head->left->nnode + 1);
}
static void subtree_print(FILE *stream, struct subtree *head, int minimal)
{
	assert(head != NULL);
	struct node_position *lines = calloc(head->nnode, sizeof(*lines));
	if (lines == NULL) {
		return;
	}

	subtree_fill_positions(head, lines, 0, 0);

	int i;
	for (i = 0; i < head->nnode; i++)
	{
		struct node_position *line = lines+i;
		int j;
		for (j = 0; j < line->depth; j++) {
			fprintf(stream, "\t");
		}

		if (line->node == NULL) {
			fprintf(stream, "NULL\n");
		} else {
			if (minimal) {
				fprintf(stream, "%p\n", line->node->key);
			} else {
				fprintf(stream, "%p (key), value:%p, node:%p, left:%p, right:%p, nnode:%d, level:%d\n", line->node->key, line->node->value, line->node, line->node->left, line->node->right, line->node->nnode, line->node->level);
			}

		}
	}
}
#endif


static void node_reconstruct_nnode(struct subtree *node)
{
	assert(node != NULL && !EOT(node));
	// NOTE: assuming nnode in children is correct

	node->nnode = node->left->nnode + 1 + node->right->nnode;
}

static void subtree_reconstruct_nnode(struct subtree *head)
{
	assert(head != NULL);
	if (EOT(head)) {
		return;
	}

	subtree_reconstruct_nnode(head->left);
	subtree_reconstruct_nnode(head->right);
	node_reconstruct_nnode(head);
}
void TestSubtree_reconstruct_nnode(CuTest *tc)
{
	struct subtree nodes[NNODE_3_LAYER_BALANCED_TREE];
	struct subtree *head = dummy_3_layer_balanced_tree(nodes);

	int i;
	for (i = 0; i < NNODE_3_LAYER_BALANCED_TREE; i++) {
		(nodes+i)->nnode = 0;
	}

	CuAssertIntEquals(tc, 0, subtree_size(head));
	subtree_reconstruct_nnode(head);
	CuAssertIntEquals(tc, NNODE_3_LAYER_BALANCED_TREE, subtree_size(head));
}


static struct subtree *subtree_rotate_right(struct subtree *head)
{
	assert(head != NULL);
	assert(!EOT(head) && !EOT(head->left));

	struct subtree *left = head->left;
	head->left = left->right;
	left->right = head;

	node_reconstruct_nnode(head);
	node_reconstruct_nnode(left);

	return left;
}
void TestSubtree_rotate_right(CuTest *tc)
{
	struct subtree nodes[NNODE_3_LAYER_BALANCED_TREE];
	struct subtree *head = dummy_3_layer_balanced_tree(nodes);

	head = subtree_rotate_right(head);
	CuAssertPtrEquals(tc, (void const *)1, head->left->key);
	CuAssertPtrEquals(tc, (void const *)2, head->key);
	CuAssertPtrEquals(tc, (void const *)3, head->right->left->key);
	CuAssertPtrEquals(tc, (void const *)4, head->right->key);
	CuAssertPtrEquals(tc, (void const *)5, head->right->right->left->key);
	CuAssertPtrEquals(tc, (void const *)6, head->right->right->key);
	CuAssertPtrEquals(tc, (void const *)7, head->right->right->right->key);

	subtree_reconstruct_nnode(head);
	CuAssertIntEquals(tc, NNODE_3_LAYER_BALANCED_TREE, subtree_size(head));
}


static struct subtree *subtree_rotate_left(struct subtree *head)
{
	assert(head != NULL);
	assert(!EOT(head) && !EOT(head->right));

	struct subtree *right = head->right;
	head->right = right->left;
	right->left = head;

	node_reconstruct_nnode(head);
	node_reconstruct_nnode(right);

	return right;
}
void TestSubtree_rotate_left(CuTest *tc)
{
	struct subtree nodes[NNODE_3_LAYER_BALANCED_TREE];
	struct subtree *head = dummy_3_layer_balanced_tree(nodes);

	head = subtree_rotate_left(head);
	CuAssertPtrEquals(tc, (void const *)1, head->left->left->left->key);
	CuAssertPtrEquals(tc, (void const *)2, head->left->left->key);
	CuAssertPtrEquals(tc, (void const *)3, head->left->left->right->key);
	CuAssertPtrEquals(tc, (void const *)4, head->left->key);
	CuAssertPtrEquals(tc, (void const *)5, head->left->right->key);
	CuAssertPtrEquals(tc, (void const *)6, head->key);
	CuAssertPtrEquals(tc, (void const *)7, head->right->key);

	subtree_reconstruct_nnode(head);
	CuAssertIntEquals(tc, NNODE_3_LAYER_BALANCED_TREE, subtree_size(head));
}


static struct subtree *skew(struct subtree *head)
{
	assert(head != NULL && !EOT(head));

	if (head->level == head->left->level) {
		subtree_rotate_right(head);
	}

	return head;
}
static struct subtree *split(struct subtree *head)
{
	assert(head != NULL && !EOT(head));

	if (head->level == head->right->right->level) {
		head = subtree_rotate_left(head);
		head->level++;
	}

	return head;
}


static struct subtree *subtree_add(struct subtree *head, tree_comparator compar, void const *key, void const *value, int *status)
{
	if (EOT(head)) {
		struct subtree *new = node_init(key, value);
		if (new == NULL) {
			*status = errno != 0 ? errno : -1;
			return &end_of_tree_sentinel;
		}
		return new;
	}

	int compared = compar(key, head->key);
	if (compared < 0) {
		head->left = subtree_add(head->left, compar, key, value, status);
#ifdef TREE_OVERWRITE_VALUE
	// WARNING: memory leakage pitfall
	// WARNING: untested
	} else if (compared == 0) {
		head->key = key;
		head->value = value;
		return head;
#endif /*TREE_OVERWRITE_VALUE*/
	} else {
		head->right = subtree_add(head->right, compar, key, value, status);
	}

	node_reconstruct_nnode(head);

	head = skew(head);
	head = split(head);

	return head;
}

int tree_add(struct tree *tree, void const *key, void const *value)
{
	if (tree == NULL || key == NULL) {
		return EINVAL;
	}

	int status = 0;
	struct subtree *head = subtree_add(tree->head, tree->compar, key, value, &status);
	if (status != 0) {
		return status;
	}

	tree->head = head;

	return 0;
}

void TestTree_add(CuTest *tc)
{
	struct tree *tree = tree_init(compare_pointers);
	CuAssertPtrNotNull(tc, tree);

	int i;
	for (i = 1; i <= 7; i++) {
		unsigned long wannabe_pointer = i;
		CuAssertIntEquals(tc, 0, tree_add(tree, (void *)wannabe_pointer, NULL));
		CuAssertIntEquals(tc, i, tree_size(tree));
	}

	// ensure it is balanced
	CuAssertPtrEquals(tc, (void const *)1, tree->head->left->left->key);
	CuAssertPtrEquals(tc, (void const *)2, tree->head->left->key);
	CuAssertPtrEquals(tc, (void const *)3, tree->head->left->right->key);
	CuAssertPtrEquals(tc, (void const *)4, tree->head->key);
	CuAssertPtrEquals(tc, (void const *)5, tree->head->right->left->key);
	CuAssertPtrEquals(tc, (void const *)6, tree->head->right->key);
	CuAssertPtrEquals(tc, (void const *)7, tree->head->right->right->key);

	tree_destroy(tree);
}


static void *subtree_search(struct subtree *head, tree_comparator compar, void const *key, int *index_of_key)
{
	assert(key != NULL);
	int le_valued_keys_skipped = 0;

	while (!EOT(head)) {
		int compared = compar(key, head->key);
		if (compared < 0) {
			head = head->left;
		} else {
			le_valued_keys_skipped += head->left->nnode;
			if (compared == 0) {
				if (index_of_key != NULL) {
					*index_of_key = le_valued_keys_skipped;
				}
				return (void *)head->value;
			} else {
				le_valued_keys_skipped += 1;
				head = head->right;
			}
		}
	}

	return NULL;
}

void *tree_search(struct tree *tree, void const *key, int *index_of_key)
{
	if (tree == NULL || key == NULL) {
		return NULL;
	}

	return subtree_search(tree->head, tree->compar, key, index_of_key);
}
void TestTree_search(CuTest *tc)
{
	struct tree *tree = tree_init(compare_pointers);
	CuAssertPtrNotNull(tc, tree);

	int index;
	CuAssertPtrEquals(tc, NULL, tree_search(tree, (void *)0xdeadbeef, &index));
	CuAssertPtrEquals(tc, NULL, tree_search(tree, (void *)0xdeadbeef, NULL));

	int i;
	for (i = 0; i < 7; i++) {
		unsigned long key = i+1;
		CuAssertPtrEquals(tc, NULL, tree_search(tree, (void *)key, &index));
		CuAssertPtrEquals(tc, NULL, tree_search(tree, (void *)key, NULL));

		CuAssertIntEquals(tc, 0, tree_add(tree, (void *)key, (void *)key));

		int j;
		for (j = 0; j < i; j++) {
		unsigned long key = j+1;
			CuAssertPtrEquals(tc, (void *)key, tree_search(tree, (void *)key, NULL));
			CuAssertPtrEquals(tc, (void *)key, tree_search(tree, (void *)key, &index));
			CuAssertIntEquals(tc, j, index);
		}
	}

	tree_destroy(tree);
}


/*
 * removes the first key-value pair where compar(key, node->key) == 0
 *
 * returns:
 *   tree == NULL || key == NULL --> EINVAL
 *   key not in tree --> ESRCH
 *   --> 0
 */
extern int tree_remove(struct tree *tree, void const *key);


static int subtree_select(struct subtree *head, tree_comparator compar, int i, void **key, void **value)
{
	assert(key != NULL && value != NULL);

	if (i < 0) {
		i += head->nnode;
	}

	while (!EOT(head)) {
		int i_head = head->left->nnode;

		if (i < i_head) {
			head = head->left;
		} else {
			if (i > i_head) {
				i -= i_head+1;
				head = head->right;
			} else {
				*key = (void *)head->key;
				*value = (void *)head->value;
				return 0;
			}
		}

	}

	return ESRCH;
}

int tree_select(struct tree *tree, int i, void **key, void **value)
{
	if (tree == NULL || key == NULL || value == NULL) {
		return EINVAL;
	}

	return subtree_select(tree->head, tree->compar, i, key, value);
}
void TestTree_select(CuTest *tc)
{
	struct tree *tree = tree_init(compare_pointers);
	CuAssertPtrNotNull(tc, tree);

	void *rkey, *rvalue;
	CuAssertIntEquals(tc, ESRCH, tree_select(tree, 0, &rkey, &rvalue));

	int i;
	for (i = 0; i < 7; i++) {
		unsigned long key = i+1;
		unsigned long value = key + 1000;
		CuAssertIntEquals(tc, ESRCH, tree_select(tree, i, &rkey, &rvalue));
		CuAssertIntEquals(tc, ESRCH, tree_select(tree, -i-1, &rkey, &rvalue));

		CuAssertIntEquals(tc, 0, tree_add(tree, (void *)key, (void *)value));

		CuAssertIntEquals(tc, 0, tree_select(tree, i, &rkey, &rvalue));
		CuAssertPtrEquals(tc, (void *)key, rkey);
		CuAssertPtrEquals(tc, (void *)value, rvalue);

		int j;
		for (j = 0; j < i; j++) {
			unsigned long key = j+1;
			unsigned long value = key + 1000;

			CuAssertIntEquals(tc, 0, tree_select(tree, j, &rkey, &rvalue));
			CuAssertPtrEquals(tc, (void *)key, rkey);
			CuAssertPtrEquals(tc, (void *)value, rvalue);

			CuAssertIntEquals(tc, 0, tree_select(tree, -i-1+j, &rkey, &rvalue));
			CuAssertPtrEquals(tc, (void *)key, rkey);
			CuAssertPtrEquals(tc, (void *)value, rvalue);
		}
	}

	tree_destroy(tree);
}


static int subtree_for_each(struct subtree *head, tree_action action, void *state)
{
	if (EOT(head)) {
		return 0;
	}

	int status = subtree_for_each(head->left, action, state);
	if (status != 0) {
		return status;
	}
	status = action(head->key, head->value, state);
	if (status != 0) {
		return status;
	}
	return subtree_for_each(head->right, action, state);
}

int tree_for_each(struct tree *tree, tree_action action, void *state)
{
	if (tree == NULL || action == NULL) {
		return EINVAL;
	}
	return subtree_for_each(tree->head, action, state);
}


struct tree_action_dump_state {
	int n;
	void const *keys[NNODE_3_LAYER_BALANCED_TREE];
	void const *values[NNODE_3_LAYER_BALANCED_TREE];
};
static int tree_action_dump(void const *key, void const *value, void *state)
{
	struct tree_action_dump_state *known_state = (struct tree_action_dump_state *)state;
	int n = known_state->n++;
	known_state->keys[n] = key;
	known_state->values[n] = value;

	return 0;
}

void TestTree_for_each(CuTest *tc)
{
	struct tree *tree = tree_init(compare_pointers);
	CuAssertPtrNotNull(tc, tree);

	int i;
	for (i = 0; i < NNODE_3_LAYER_BALANCED_TREE; i++) {
		unsigned long key = i+1;
		unsigned long value = key+1000;

		CuAssertIntEquals(tc, 0, tree_add(tree, (void *)key, (void *)value));

		struct tree_action_dump_state state;
		state.n = 0;
		tree_for_each(tree, tree_action_dump, &state);
		CuAssertIntEquals(tc, i+1, state.n);

		int j;
		for (j = 0; j < i; j++) {
			unsigned long key = j+1;
			unsigned long value = key+1000;

			CuAssertPtrEquals(tc, (void *)key, state.keys[j]);
			CuAssertPtrEquals(tc, (void *)value, state.values[j]);
		}
	}
}


struct tree_action_abort_after_state {
	int ncall;
	int abort_after;
};
static int tree_action_abort_after(void const *key, void const *value, void *state)
{
	struct tree_action_abort_after_state *known_state = (struct tree_action_abort_after_state *)state;
	known_state->ncall++;
	return known_state->ncall >= known_state->abort_after ? known_state->abort_after : 0;
}

void TestTree_for_eachWithAbortion(CuTest *tc)
{
	struct tree *tree = tree_init(compare_pointers);
	CuAssertPtrNotNull(tc, tree);

	int i;
	for (i = 0; i < NNODE_3_LAYER_BALANCED_TREE; i++) {
		unsigned long key = i+1;
		CuAssertIntEquals(tc, 0, tree_add(tree, (void *)key, (void *)key));

		int j;
		for (j = 1; j < i; j++) { // cannot abort before the zeroth call
			struct tree_action_abort_after_state state;
			state.ncall = 0;
			state.abort_after = j;
			CuAssertIntEquals(tc, j, tree_for_each(tree, tree_action_abort_after, &state));

			CuAssertIntEquals(tc, j, state.ncall);
		}


	}
}
