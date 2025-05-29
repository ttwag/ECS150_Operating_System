CC = gcc
CFLAGS = -Wall -Wextra -I./ #-Werror
SRC_DIR = .
BUILD_DIR = .
SRCS = $(wildcard $(SRC_DIR)/*.c)
OBJS = $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(SRCS))
TARGET = $(BUILD_DIR)/libuthread.a

# For macOS compatibility (optional)
ifeq ($(shell uname -s),Darwin)
    CFLAGS += -Wno-deprecated-declarations
endif

all: $(TARGET)

$(TARGET): $(OBJS)
	ar rcs $@ $(OBJS)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(BUILD_DIR)/*.o $(TARGET)

.PHONY: all clean