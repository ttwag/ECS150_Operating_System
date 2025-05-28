CC = gcc
CFLAGS = -Wall -Wextra -I./include #-Werror
SRC_DIR = ./src
BUILD_DIR = ./build
SRCS = $(wildcard $(SRC_DIR)/*.c)
OBJS = $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(SRCS))
TARGET = $(BUILD_DIR)/app

# For macOS compatibility (optional)
ifeq ($(shell uname -s),Darwin)
    CFLAGS += -Wno-deprecated-declarations
endif

all: $(TARGET)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(TARGET): $(OBJS) | $(BUILD_DIR)
	$(CC) $(CFLAGS) $^ -o $@

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(BUILD_DIR)/*

run: $(TARGET)
	$(TARGET)

debug: CFLAGS += -g
debug: clean all
	gdb $(TARGET)

.PHONY: all clean run debug