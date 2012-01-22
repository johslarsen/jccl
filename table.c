#include "table.h"
#include <errno.h>
#include <stdlib.h>
#include <string.h>

typedef struct Table_node Table_node;
struct Table_node {
	Table_node	*next; // next node in hash chain
	char		*key;
	void		*value;
};

struct Table {
	Table_node 		**entries; // hash table
	unsigned int	size; // size of hash table
};


Table *table_init(unsigned int size)
{
	if (size == 0) {
		return NULL;
	}

	Table *table = (Table *)malloc(sizeof(*table));
	if (table == NULL) {
		return NULL;
	}

	table->size = size;
	table->entries = NULL;

	table->entries = (Table_node **)calloc(sizeof(*table->entries), table->size);
	if (table->entries == NULL) {
		table_free(table);
		return NULL;
	}

	return table;
}


int table_free(Table *table)
{
	if (table == NULL) {
		return EINVAL;
	}

	if (table->entries != NULL) {
		int i;
		for (i = 0; i < table->size; i++) {
			// remove each node in the i-th hash chain
			Table_node *nnp, *np; // next - / node pointer
			for (np = table->entries[i]; np != NULL; np = nnp) {
				nnp = np->next;
				free(np->key);
				free(np);
			}
		}
	}

	free(table);
	
	return 0;
}


/* creates the hash for the key */
static unsigned int hash(const Table *table, const char *key)
{
	enum {
		Multiplier = 37, // empirically good value, see page 56 of Brian W. Kernighan, Rob Pike. "The pracitice of programming"
	};

	unsigned int hash;
	const unsigned char *c;

	for (hash = 0, c = (const unsigned char *)key; *c != '\0'; c++) {
		hash = Multiplier * hash + *c;
	}

	return hash % table->size;
}


/* allocates memory for a table node and sets its fields */
static Table_node *table_create_node(const char *key, void *value)
{
	Table_node *np = malloc(sizeof(*np)); // node pointer
	if (np == NULL) {
		return NULL;
	}

	np->next = NULL;
	np->key = strdup(key);
	np->value = value;

	return np;
}


int table_add(Table *table, const char *key, void *value)
{
	if (table == NULL) {
		return EINVAL;
	}

	Table_node *np = table_create_node(key, value); // node pointer
	if (np == NULL) {
		return errno;
	}

	unsigned int h = hash(table, key);
	np->next = table->entries[h];
	table->entries[h] = np;

	return 0;
}


int table_remove(Table *table, const char *key)
{
	if (table == NULL) {
		return EINVAL;
	}

	unsigned int h = hash(table, key);
	Table_node *np; // node pointer

	if (table->entries[h] == NULL) {
		// empty hash chain
		return EINVAL;
	} else if (strcmp(table->entries[h]->key, key) == 0) {
		// key found at head of hash chain
		np = table->entries[h];
		table->entries[h] = np->next;
	} else {
		Table_node *pnp; // previous node pointer

		// traverse hash chain to find key
		for (pnp = table->entries[h]; pnp->next != NULL; pnp=pnp->next) {
			if (strcmp(pnp->next->key, key) == 0) {
				break;
			}
		}

		if (pnp->next == NULL) {
			// did not find key in hash chain
			return EINVAL;
		}

		np = pnp->next;
		pnp->next = np->next;
	}

	free(np->key);
	free(np);

	return 0;
}


void *table_lookup(Table *table, const char *key)
{
	if (table == NULL) {
		return NULL;
	}

	unsigned int h = hash(table, key);
	Table_node *np; // node pointer
	for (np = table->entries[h]; np != NULL; np = np->next) {
		if (strcmp(np->key, key) == 0) {
			return np->value;
		}
	}

	return NULL;
}

