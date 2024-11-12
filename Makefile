TARGET_EXEC := main
BIN_DIR := ./bin
SRC_DIRS := ./src
SRCS := $(shell find $(SRC_DIRS) -name '*.c')
INC_DIRS := $(shell find $(SRC_DIRS) -type d)
INC_FLAGS := $(addprefix -I,$(INC_DIRS))
CC = gcc
CFLAGS = $(INC_FLAGS) -lraylib
CPPFLAGS = -MMD -MP

$(BIN_DIR)/$(TARGET_EXEC): $(SRCS)
	mkdir -p bin/
	$(CC) $(CPPFLAGS) $(CFLAGS) $^ -o $@

clean:
	rm -r $(BIN_DIR)/
