#include "../table.h"
#include "../unittest.h"
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>

void table_test(void)
{
	enum {
		Nkey = 5,
	};

	char *somekey = "key";
	int somevalue = rand();

	UNITTEST(table_create(0) == NULL);
	UNITTEST(table_destroy(NULL) == EINVAL);
	UNITTEST(table_add(NULL, somekey, &somevalue) == EINVAL);
	UNITTEST(table_remove(NULL, somekey) == EINVAL);
	UNITTEST(table_lookup(NULL, somekey) == NULL);

	Table *table = table_create(Nkey - 1); // when table is smaller than the number of keys there have to be at least 1 hash chain with more than 1 key
	if (table == NULL) {
		fprintf(stderr, "table_test: can not create table, aborting\n");
		return;
	}

	char *keys[Nkey] = {"key0","key1","key2","key3","key4"};
	int values[Nkey];

	int i;
	for (i = 0; i < Nkey; i++) {
		values[i] = rand();
		UNITTEST(table_add(table, keys[i], &values[i]) == 0);
	}

	// somekey is not in chan
	UNITTEST(table_lookup(table, somekey) == NULL);
	UNITTEST(table_remove(table, somekey) == EINVAL);

	for (i = 0; i < Nkey; i++) {
		UNITTEST(table_lookup(table, keys[i]) == &values[i]);
	}

	for (i = 0; i < Nkey; i++) {
		UNITTEST(table_remove(table, keys[i]) == 0);
		UNITTEST(table_lookup(table, keys[i]) == NULL);
	}
	
	UNITTEST(table_destroy(table) == 0);
}

int main(void)
{
	table_test();
	
	return 0;
}
