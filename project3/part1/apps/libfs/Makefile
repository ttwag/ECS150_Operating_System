CC = gcc
CFLAGS = -Wall -Wextra -I./ -Werror
DEBUG_CFLAGS = -Wall -Wextra -I./ -fsanitize=address -static-libasan -g
SRC_DIR = .
BUILD_DIR = .
SRCS = $(wildcard $(SRC_DIR)/*.c)
OBJS = $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(SRCS))
TARGET = $(BUILD_DIR)/libfs.a

ifeq ($(DEBUG),1)
    CFLAGS = $(DEBUG_CFLAGS)
endif

all: $(TARGET)

$(TARGET): $(OBJS)
	ar rcs $@ $(OBJS)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(BUILD_DIR)/*.o $(TARGET)

debug : CFLAGS = $(DEBUG_CFLAGS)
debug: clean all run
	@echo "Built the debug version"

.PHONY: all clean debug