# TODO: refactor and cleanup
.PHONY: clean unit_test unit_test_dev all obj install uninstall lint valgrind
.DELETE_ON_ERROR:

CC             ?= gcc
AR             ?= ar
LINTER         ?= clang-format

LIB            := libutil

PREFIX         := /usr/local
INCDIR         := $(PREFIX)/include
LIBDIR         := $(PREFIX)/lib
SRCDIR         := src
DEPSDIR        := deps
TESTDIR        := t
EXAMPLEDIR     := examples
LINCDIR        := include

DYNAMIC_TARGET := $(LIB).so
STATIC_TARGET  := $(LIB).a
EXAMPLE_TARGET := example
TEST_TARGET    := test

SRC       := $(wildcard $(SRCDIR)/*.c)
TEST_DEPS := $(wildcard $(DEPSDIR)/libtap/*.c)
DEPS      := $(filter-out $(wildcard $(DEPSDIR)/libtap/*), $(wildcard $(DEPSDIR)/*/*.c))
OBJ       := $(addprefix obj/, $(notdir $(SRC:.c=.o)) $(notdir $(DEPS:.c=.o)))

STRICT    := -Wall -Werror -Wextra -Wno-missing-field-initializers \
 -Wmissing-prototypes -Wstrict-prototypes -Wold-style-definition \
 -Wno-unused-parameter -Wno-unused-function -Wno-unused-value \

CFLAGS    := -I$(LINCDIR) -I$(DEPSDIR) -pedantic -Wno-error=incompatible-pointer-types -std=c17
LIBS      := -lm

TESTS     := $(wildcard $(TESTDIR)/*.c)

all: $(DYNAMIC_TARGET) $(STATIC_TARGET)

$(DYNAMIC_TARGET): $(OBJ)
	$(CC) $(CFLAGS) $(STRICT) $(OBJ) -shared $(LIBS) -o $(DYNAMIC_TARGET)

$(STATIC_TARGET): $(OBJ)
	$(AR) rcs $@ $(OBJ)

obj/%.o: $(SRCDIR)/%.c $(LINCDIR)/$(LIB).h | obj
	$(CC) $< -c $(CFLAGS) $(STRICT) -o $@

obj/%.o: $(DEPSDIR)/*/%.c | obj
	$(CC) $< -c $(CFLAGS) $(STRICT) -o $@

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

unit_test: $(STATIC_TARGET)
	$(CC) $(CFLAGS) $(TESTS) $(TEST_DEPS) $(STATIC_TARGET) -I$(SRCDIR) $(LIBS) -o $(TEST_TARGET)
	./$(TEST_TARGET)
	$(MAKE) clean

unit_test_dev:
	ls $(SRCDIR)/*.{h,c} $(TESTDIR)/*.{h,c} | entr -s 'make -s unit_test'

valgrind: $(STATIC_TARGET)
	$(CC) $(CFLAGS) $(TESTS) $(TEST_DEPS) $(STATIC_TARGET) -I$(SRCDIR) $(LIBS) -o $(TEST_TARGET)
	valgrind --leak-check=full --track-origins=yes -s ./$(TEST_TARGET)
	$(MAKE) clean

lint:
	$(LINTER) -i $(wildcard $(SRCDIR)/*) $(wildcard $(TESTDIR)/*) $(wildcard $(LINCDIR)/*) $(wildcard $(EXAMPLEDIR)/*)
