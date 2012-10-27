CC             := gcc
CFLAGS         := -pipe -g -p -Wall

RM             := rm -f

LIBS           := -lm

CUTEST_SRC     := CuTest/CuTest.c
TEST_SUITE     := AllTests
TEST_SUITE_SRC := $(TEST_SUITE).c

HEADERS        := $(wildcard *.h)
SOURCES        := $(wildcard *.c) $(CUTEST_SRC) $(TEST_SUITE_SRC)
OBJECTS        := $(patsubst %.c, %.o, $(SOURCES))

TEST_SUITE_GENERATOR := CuTest/make-tests.sh

.PHONY: clean

all: test
test: $(TEST_SUITE)
	./$<

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

$(TEST_SUITE_SRC):
	$(TEST_SUITE_GENERATOR) > $@

$(TEST_SUITE): $(OBJECTS) $(HEADERS)
	$(CC) $(CFLAGS) $^ -o $@ $(LIBS)
	$(RM) $(TEST_SUITE_SRC)

clean:
	$(RM) $(TEST_SUITE) $(TEST_SUITE_SRC) $(OBJECTS)

