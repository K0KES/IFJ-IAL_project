SHELL := /bin/bash

TARGET_EXEC ?= IFJ_compiler.out

BUILD_DIR ?= ./build
SRC_DIRS ?= ./src
TEST_DIR ?= ./tests

CC := gcc

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

# regular tests
test_eparser: 
	$(TEST_CC) -o my_gtest_test_expression_parser $(TEST_DIR)/expression_parser_tests/main.cpp -lgtest -lgtest_main -g
# ./my_gtest_test_expression_parser < $(TEST_DIR)/expression_parser_tests/test1.txt
	# ./my_gtest_test_expression_parser

test_parser: 
	$(TEST_CC) -o my_gtest_test_parser $(TEST_DIR)/parser_tests/main.cpp -lgtest -lgtest_main -g
	./my_gtest_test_parser

test_scanner: 
	$(TEST_CC) -o my_gtest_test_scanner $(TEST_DIR)/scanner_tests/main.cpp -lgtest -lgtest_main -g
	./my_gtest_test_scanner

test_symtable: 
	$(TEST_CC) -o my_gtest_test_symtable $(TEST_DIR)/symtable_tests/main.cpp -lgtest -lgtest_main -g
	./my_gtest_test_symtable

# memory leaks test
valgrind_eparser: 
	$(TEST_CC) -o my_gtest_test_expression_parser $(TEST_DIR)/expression_parser_tests/main.cpp -lgtest -lgtest_main -g
	valgrind --leak-check=full --show-leak-kinds=all ./my_gtest_test_expression_parser

valgrind_parser: 
	$(TEST_CC) -o my_gtest_test_parser $(TEST_DIR)/parser_tests/main.cpp -lgtest -lgtest_main -g
	valgrind --leak-check=full --show-leak-kinds=all ./my_gtest_test_parser

valgrind_scanner: 
	$(TEST_CC) -o my_gtest_test_scanner $(TEST_DIR)/scanner_tests/main.cpp -lgtest -lgtest_main -g
	valgrind --leak-check=full --show-leak-kinds=all ./my_gtest_test_scanner

valgrind_symtable: 
	$(TEST_CC) -o my_gtest_test_symtable $(TEST_DIR)/symtable_tests/main.cpp -lgtest -lgtest_main -g
	valgrind --leak-check=full --show-leak-kinds=all ./my_gtest_test_symtable

odevzdani:
	mkdir -p odevzdani
	mkdir -p test_dir
	cp src/* odevzdani/
	cp Makefile_for_odevzdani odevzdani/Makefile
	cp rozdeleni odevzdani/rozdeleni
	cp rozsireni odevzdani/rozsireni
	cp dokumentace.pdf odevzdani/dokumentace.pdf
	zip -j xvalao01.zip odevzdani/*
	rmdir --ignore-fail-on-non-empty test_dir
	./is_it_ok.sh xvalao01.zip test_dir/
	sudo $(RM) -r test_dir
	$(RM) -r odevzdani




.PHONY: clean odevzdani

clean:
	$(RM) -r $(BUILD_DIR)
	$(RM) my_gtest_test_*
	$(RM) xvalao01.zip

MKDIR_P ?= mkdir -p
