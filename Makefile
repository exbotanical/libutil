# Directories
SRC_DIR := src
DEP_DIR := deps

# Compiler
CC := gcc
CFLAGS := -Wall -Wextra -pedantic -std=c17

# Targets
TARGET := libutil.a
EXAMPLE_TARGET := example
TEST_TARGET := test

# Metadata
LINK_NAME := $(patsubst lib%,%,$(patsubst %.a,%, $(TARGET)))

# Source files
SRCS := $(wildcard $(SRC_DIR)/*.c) $(wildcard $(DEP_DIR)/*/*.c)

# Object files
OBJS := $(patsubst %.c,%.o,$(SRCS))

# Test files
TESTS := $(wildcard t/*.c)

# Constants
SEPARATOR := ---------------------------

# Rule to build object files from source files
%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

%.o: $(DEP_DIR)/%/%.c
	$(CC) $(CFLAGS) -c $< -o $@

# Rule to create the static library
$(TARGET): $(OBJS)
	ar rcs $@ $^
	rm $(OBJS)

# Rule to build the example executable
$(EXAMPLE_TARGET): examples/main.c $(TARGET)
	$(CC) -I$(SRC_DIR) -I$(DEP_DIR) $(CFLAGS) $< -L./ -l$(LINK_NAME) -o $@

.PHONY: clean

clean:
	rm -f $(OBJS) $(TARGET) $(EXAMPLE_TARGET) $(TEST_TARGET)

# `make -s test`
test: $(TARGET)
	$(foreach test,$(TESTS),					  																											\
		$(MAKE) .compile_test file=$(test); 																										\
		printf "\033[1;32m\nRunning test $(patsubst t/%,%,$(test))...\n$(SEPARATOR)\n\033[0m";	\
		./test;\
 	)
	rm $(TEST_TARGET)

.compile_test:
	gcc -D debug -I$(DEP_DIR) -I$(SRC_DIR) $(file) -o $(TEST_TARGET) -L./ -l$(LINK_NAME)
