LIBUTHREAD_DIR = ./libuthread
CC = gcc
CFLAGS = -Wall -Wextra -I$(LIBUTHREAD_DIR) -Werror
DEBUG_CFLAGS = -Wall -Wextra -I$(LIBUTHREAD_DIR) -fsanitize=address -g

DEBUG = 0
SRC_DIR = .
BUILD_DIR = .
SRCS = $(wildcard $(SRC_DIR)/*.c)
OBJS = $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(SRCS))
TARGET = $(BUILD_DIR)/app

all: $(TARGET)

# link the object files and lib
$(TARGET): $(OBJS) $(LIBUTHREAD_DIR)/libuthread.a
	$(CC) $(CFLAGS) -o $@ $< -L$(LIBUTHREAD_DIR) -luthread

# compile the apps/*.c to object files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@ 

# build the library first
$(LIBUTHREAD_DIR)/libuthread.a:
	$(MAKE) -C $(LIBUTHREAD_DIR) DEBUG=$(DEBUG)

clean:
	$(MAKE) -C $(LIBUTHREAD_DIR) clean
	rm -f *.o app

run: $(TARGET)
	$(TARGET)

debug: DEBUG = 1
debug: CFLAGS = $(DEBUG_CFLAGS)
debug: clean all run
	@echo "Built the debug version"

.PHONY: all clean run debug