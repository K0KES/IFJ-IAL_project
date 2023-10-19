TARGET_EXEC ?= IFJ_compiler.out

BUILD_DIR ?= ./build
SRC_DIRS ?= ./src
TEST_DIR ?= ./tests

SRCS := $(shell find $(SRC_DIRS) -name *.c)
OBJS := $(SRCS:%=$(BUILD_DIR)/%.o)
CC := gcc
TEST_CC := g++

default: $(BUILD_DIR)/$(TARGET_EXEC)

CFLAGS := -Wall -Wextra -std=c99 -g -O0 -DDEBUG -pedantic

# CFLAGS := -Wall -Wextra -Werror -pedantic -std=c99 -g -O0 -DDEBUG

$(BUILD_DIR)/$(TARGET_EXEC): $(OBJS)
	$(CC) $(OBJS) -o $@ 

# c source
$(BUILD_DIR)/%.c.o: %.c %.h
	$(MKDIR_P) $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

test: $(BUILD_DIR)/$(TARGET_EXEC)
	$(TEST_CC) -o my_gtest_test $(TEST_DIR)/expression_parser_tests/main.cpp -lgtest -lgtest_main -Wall -Wextra
	./my_gtest_test

valgrind: $(BUILD_DIR)/$(TARGET_EXEC)
	$(TEST_CC) -o my_gtest_test $(TEST_DIR)/expression_parser_tests/main.cpp -lgtest -lgtest_main -Wall -Wextra -g
	valgrind --leak-check=full --show-leak-kinds=all ./my_gtest_test

.PHONY: clean test

clean:
	$(RM) -r $(BUILD_DIR)
	$(RM) my_gtest_test


MKDIR_P ?= mkdir -p
