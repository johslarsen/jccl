#ifndef TABLE_H
#define TABLE_H

typedef struct Table Table;

/* create a hash table. ORDER=1 
 * size is the number of unique hashes 
 * return NULL on error */
Table *table_init(unsigned int size);

/* free the memory allocated for a hash table ORDER=n
 * return != 0 on error */
int table_free(Table *table);

/* add a key/value pair to the table. ORDER=i, i=number of keys with the same hash
 * the key will be duplicated, but the value is only a pointer
 * return != 0 on error */
int table_add(Table *table, const char *key, void *value);

/* remove a key/value pair. ORDER=i, i=its index in the hash chain
 * return != 0 on error */
int table_remove(Table *table, const char *key);

/* return the value that the key points to ORDER=i, i=its index in the hash chain 
 * return NULL on error */
void *table_lookup(Table *table, const char *key);


#endif /* TABLE_H */
