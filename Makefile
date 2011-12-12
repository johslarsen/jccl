CC		= gcc

CFLAGS  = -pipe
CFLAGS += -g
#CFLAGS += -p
CFLAGS += -Wall
#CFLAGS += -O2



.PHONY: check
check: dmath.test list.test sort.test table.test
	for i in *.test; do echo "$$i:"; ./"$$i" 2>&1 | sed 's/^/\t/'; done

.PHONY: clean
clean:
	\rm -f *.o test/*.o *.test

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

list.test: list.o test/list.o unittest.o
	$(CC) $(CFLAGS) $^ -o $@
dmath.test: dmath.o test/dmath.o unittest.o
	$(CC) $(CFLAGS) $^ -o $@ -lm
sort.test: sort.o test/sort.o unittest.o
	$(CC) $(CFLAGS) $^ -o $@
table.test: table.o test/table.o unittest.o
	$(CC) $(CFLAGS) $^ -o $@ -lm
