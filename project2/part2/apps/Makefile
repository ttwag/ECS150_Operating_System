LIBUTHREAD_DIR = ./libuthread
CC = gcc
CFLAGS = -Wall -Wextra -I$(LIBUTHREAD_DIR) #-Werror
SRC_DIR = .
BUILD_DIR = .
SRCS = $(wildcard $(SRC_DIR)/*.c)
OBJS = $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(SRCS))
TARGET = $(BUILD_DIR)/app

all: $(TARGET)

$(TARGET): $(OBJS) $(LIBUTHREAD_DIR)/libuthread.a
	$(CC) $(CFLAGS) -o $@ $< -L$(LIBUTHREAD_DIR) -luthread

# compile the apps/*.c
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@ 

# build the library first
$(LIBUTHREAD_DIR)/libuthread.a:
	$(MAKE) -C $(LIBUTHREAD_DIR)

clean:
	$(MAKE) -C $(LIBUTHREAD_DIR) clean
	rm -f *.o app

run: $(TARGET)
	$(TARGET)

.PHONY: all clean run