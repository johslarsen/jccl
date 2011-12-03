CC		= gcc

CFLAGS  = -pipe
CFLAGS += -g
#CFLAGS += -p
CFLAGS += -Wall
#CFLAGS += -O2



.PHONY: check
check: dmath.test list.test table.test
	for i in *.test; do ./$$i; done

.PHONY: clean
clean:
	\rm -f *.o test/*.o *.test

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

list.test: list.o test/list.o unittest.o
	$(CC) $(CFLAGS) $^ -o $@
dmath.test: dmath.o test/dmath.o unittest.o
	$(CC) $(CFLAGS) $^ -o $@ -lm
table.test: table.o test/table.o unittest.o
	$(CC) $(CFLAGS) $^ -o $@ -lm
