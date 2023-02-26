# Directories
SRC_DIR := src
DEP_DIR := deps

# Compiler
CC := gcc
CFLAGS := -Wall -Wextra -pedantic -std=c17

# Targets
TARGET := libutil.a
EXAMPLE_TARGET := example

# Source files
SRCS := $(wildcard $(SRC_DIR)/*.c) $(wildcard $(DEP_DIR)/*/*.c)

# Object files
OBJS := $(patsubst %.c,%.o,$(SRCS))

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
	$(CC) -Isrc -Ideps $(CFLAGS) $< -L./ -lutil -o $@

.PHONY: clean

clean:
	rm -f $(OBJS) $(TARGET) $(EXAMPLE_TARGET)

# TESTS := $(patsubst %.c, %, $(wildcard t/*.c))

# # `make -s test` for cleaner output
# test:
# 	./scripts/test.bash
# 	$(MAKE) clean
