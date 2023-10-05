TARGET_EXEC ?= IFJ_compiler.out

BUILD_DIR ?= ./build
SRC_DIRS ?= ./src

SRCS := $(shell find $(SRC_DIRS) -name *.c)
OBJS := $(SRCS:%=$(BUILD_DIR)/%.o)

default: $(BUILD_DIR)/$(TARGET_EXEC)

CFLAGS := -Wall -Wextra -std=c99 -g -O0 -DDEBUG -pedantic

# CFLAGS := -Wall -Wextra -Werror -pedantic -std=c99 -g -O0 -DDEBUG

$(BUILD_DIR)/$(TARGET_EXEC): $(OBJS)
	$(CC) $(OBJS) -o $@ 

# c source
$(BUILD_DIR)/%.c.o: %.c
	$(MKDIR_P) $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

test: $(BUILD_DIR)/$(TARGET_EXEC)


.PHONY: clean test

clean:
	$(RM) -r $(BUILD_DIR)


MKDIR_P ?= mkdir -p
