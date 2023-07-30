CC ?= gcc
AR ?= ar
LINTER ?= clang-format

LIB := libutil

PREFIX := /usr/local
INCDIR := $(PREFIX)/include
LIBDIR := $(PREFIX)/lib
SRCDIR := src
DEPSDIR := deps
TESTDIR := t
EXAMPLEDIR := examples
LINCDIR := include

DYNAMIC_TARGET := $(LIB).so
STATIC_TARGET := $(LIB).a
EXAMPLE_TARGET := example
TEST_TARGET := test

SRC := $(wildcard $(SRCDIR)/*.c)
TEST_DEPS := $(wildcard $(DEPSDIR)/tap.c/*.c)
DEPS := $(filter-out $(wildcard $(DEPSDIR)/tap.c/*), $(wildcard $(DEPSDIR)/*/*.c))
OBJ := $(addprefix obj/, $(notdir $(SRC:.c=.o)) $(notdir $(DEPS:.c=.o)))

CFLAGS := -I$(LINCDIR) -I$(DEPSDIR) -Wall -Wextra -pedantic -std=c17
LIBS := -lm

TESTS := $(wildcard $(TESTDIR)/*.c)

SEPARATOR := ---------------------------

all: $(DYNAMIC_TARGET) $(STATIC_TARGET)

$(DYNAMIC_TARGET): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -shared $(LIBS) -o $(DYNAMIC_TARGET)

$(STATIC_TARGET): $(OBJ)
	$(AR) rcs $@ $(OBJ)

obj/%.o: $(SRCDIR)/%.c $(LINCDIR)/$(LIB).h | obj
	$(CC) $< -c $(CFLAGS) -o $@

obj/%.o: $(DEPSDIR)/*/%.c | obj
	$(CC) $< -c $(CFLAGS) -o $@

obj:
	mkdir -p obj

install: $(STATIC_TARGET)
	mkdir -p ${LIBDIR} && cp -f ${STATIC_TARGET} ${LIBDIR}/$(STATIC_TARGET)
	mkdir -p ${INCDIR} && cp -r $(LINCDIR)/$(LIB).h ${INCDIR}

uninstall:
	rm -f ${LIBDIR}/$(STATIC_TARGET)
	rm -f ${INCDIR}/libys.h

$(EXAMPLE_TARGET): $(STATIC_TARGET)
	$(CC) $(CFLAGS) $(EXAMPLEDIR)/main.c $(STATIC_TARGET) $(LIBS) -o $(EXAMPLE_TARGET)

clean:
	rm -f $(OBJ) $(STATIC_TARGET) $(DYNAMIC_TARGET) $(EXAMPLE_TARGET) $(TEST_TARGET)

test: $(STATIC_TARGET)
	$(foreach test,$(TESTS),					  				\
		$(MAKE) .compile_test file=$(test); 			\
		./test;																		\
 	)
	rm $(TEST_TARGET)

valgrind: $(STATIC_TARGET)
	$(foreach test,$(TESTS),					  												\
		$(MAKE) .compile_test file=$(test); 											\
		valgrind --leak-check=full --track-origins=yes -s ./test  \
	)
	rm $(TEST_TARGET)

.compile_test:
	$(CC) $(CFLAGS) $(file) $(TEST_DEPS) $(STATIC_TARGET) -I$(SRCDIR) -I$(DEPSDIR) $(LIBS) -o $(TEST_TARGET)

lint:
	$(LINTER) -i $(wildcard $(SRCDIR)/*) $(wildcard $(TESTDIR)/*) $(wildcard $(LINCDIR)/*) $(wildcard $(EXAMPLEDIR)/*)

.PHONY: clean test .compile_test all obj install uninstall lint
