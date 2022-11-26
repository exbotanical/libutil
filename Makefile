CC=gcc
CFLAGS=-g -fPIC -Ideps -Wall -Wextra -pedantic -std=c17
LDFLAGS=-shared -o

BIN=libutil.so

SRC=$(wildcard src/*.c)
DEPS=$(wildcard deps/*/*.c)
TESTS = $(patsubst %.c, %, $(wildcard t/*.c))

all:
	$(CC) $(CFLAGS) $(SRC) $(DEPS) $(LDFLAGS) $(BIN)

clean:
	rm -f $(SRC:.c=.o) $(BIN)

test:
	./scripts/test.bash
	$(MAKE) clean

.PHONY: test clean
