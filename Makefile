CC		= gcc

CFLAGS  = -pipe
CFLAGS += -g
#CFLAGS += -p
CFLAGS += -Wall
#CFLAGS += -O2



.PHONY: check
check: dmath.test list.test
	for i in *.test; do ./$$i; done

.PHONY: clean
clean:
	\rm -f *.o test/*.o *.test

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

list.test: list.o test/list.o
	$(CC) $(CFLAGS) $^ -o $@
dmath.test: dmath.o test/dmath.o
	$(CC) $(CFLAGS) $^ -o $@ -lm
