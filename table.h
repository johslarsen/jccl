#ifndef TABLE_H
#define TABLE_H

typedef struct Table Table;

/* create a hash table */
Table *table_init(unsigned int size);

/* free the memory allocated for a hash table
 * nodes and keys are freed, but the value they points to are preserved */
int table_free(Table *table);

/* add a key/value pair to the table
 * the key will be duplicated, but the value is only a pointer */
int table_add(Table *table, const char *key, void *value);

/* remove a key/value pair
 * the node and the key are freed, but the value it points to is preserved */
int table_remove(Table *table, const char *key);

/* return the value that the key points to */
void *table_lookup(Table *table, const char *key);


#endif /* TABLE_H */
