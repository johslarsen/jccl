CC				= gcc
COMPILE			= -pipe
#OPTIMALIZATION	= -O2
DEBUG			= -pg
WARNINGS		= -Wall
LIBS			= -lm

CFLAGS = $(COMPILE) $(OPTIMALIZATION) $(DEBUG) $(WARNINGS)

SRCS	= dmath.c list.c
HEADERS	= $(SRCS:.c=.h)
OBJS	= $(SRCS:.c=.o)

TEST_SRCS = $(SRCS:^=test/)
TEST_OBJS = $(TEST_SRCS:.c=.o)

.PHONY: check
check: dmath.test list.test
	for i in *.test; do ./$$i; done

.PHONY: clean
clean:
	\rm -f *.o test/*.o *.test

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $< $(LIBS)

list.test: list.o test/list.o
	$(CC) $(CFLAGS) $^ -o $@
dmath.test: dmath.o test/dmath.o
	$(CC) $(CFLAGS) $^ -o $@ -lm
