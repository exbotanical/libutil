include Makefile.config

.PHONY: clean unit_test unit_test_dev all obj install uninstall fmt valgrind
.DELETE_ON_ERROR:

SRCDIR         := src
DEPSDIR        := deps
TESTDIR        := t
EXAMPLEDIR     := examples
LINCDIR        := include

DYNAMIC_TARGET := $(LIBNAME).so
STATIC_TARGET  := $(LIBNAME).a
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

obj/%.o: $(SRCDIR)/%.c $(LINCDIR)/*.h | obj
	$(CC) $< -c $(CFLAGS) $(STRICT) -o $@

obj/%.o: $(DEPSDIR)/*/%.c | obj
	$(CC) $< -c $(CFLAGS) $(STRICT) -o $@

obj:
	@mkdir -p obj

install: $(LINCDIR)/*.h | $(STATIC_TARGET)
	$(shell mkdir -p $(INSTALL_DIR)/lib)
	$(shell mkdir -p $(INSTALL_DIR)/include/$(LIBNAME))
	$(INSTALL) $(STATIC_TARGET) $(INSTALL_DIR)/lib
	$(INSTALL) $^ $(INSTALL_DIR)/include/$(LIBNAME)
	$(INSTALL) $(MANPAGE) $(MAN_DIR)/$(MANPAGE)

uninstall:
	$(shell rm $(INSTALL_DIR)/lib/$(STATIC_TARGET))
	$(shell rm -rf $(INSTALL_DIR)/include/$(LIBNAME))
	$(shell rm $(MAN_DIR)/$(MANPAGE))

$(EXAMPLE_TARGET): $(STATIC_TARGET)
	$(CC) $(CFLAGS) $(EXAMPLEDIR)/main.c $(STATIC_TARGET) $(LIBS) -o $(EXAMPLE_TARGET)

clean:
	@rm -f $(OBJ) $(STATIC_TARGET) $(DYNAMIC_TARGET) $(EXAMPLE_TARGET) $(TEST_TARGET)

unit_test: $(STATIC_TARGET)
	$(CC) $(CFLAGS) $(TESTS) $(TEST_DEPS) $(STATIC_TARGET) -I$(SRCDIR) $(LIBS) -o $(TEST_TARGET)
	./$(TEST_TARGET)
	$(MAKE) clean

unit_test_dev:
	@ls $(SRCDIR)/*.{h,c} $(TESTDIR)/*.{h,c} | entr -s 'make -s unit_test'

valgrind: $(STATIC_TARGET)
	$(CC) $(CFLAGS) $(TESTS) $(TEST_DEPS) $(STATIC_TARGET) -I$(SRCDIR) $(LIBS) -o $(TEST_TARGET)
	@valgrind --leak-check=full --track-origins=yes -s ./$(TEST_TARGET)
	$(MAKE) clean

fmt:
	$(FMT) -i $(wildcard $(SRCDIR)/*) $(wildcard $(TESTDIR)/*) $(wildcard $(LINCDIR)/*) $(wildcard $(EXAMPLEDIR)/*)
