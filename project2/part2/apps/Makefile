LIBUTHREAD_DIR = ./libuthread
CC = gcc
CFLAGS = -Wall -Wextra -I$(LIBUTHREAD_DIR) -Werror
DEBUG_CFLAGS = -Wall -Wextra -I$(LIBUTHREAD_DIR) -fsanitize=address -g

DEBUG = 0
SRC_DIR = .
BUILD_DIR = .

QUEUE_TESTER = $(BUILD_DIR)/queue_tester
QT_SRC = queue_tester.c
QT_OBJ = queue_tester.o

UTHREAD_HELLO = $(BUILD_DIR)/uthread_hello
UH_SRC = uthread_hello.c
UH_OBJ = uthread_hello.o


TARGET = $(BUILD_DIR)/app

all: $(QUEUE_TESTER)

# queue_test: link the object files and lib
$(QUEUE_TESTER): $(QT_OBJ) $(LIBUTHREAD_DIR)/libuthread.a
	$(CC) $(CFLAGS) -o $@ $< -L$(LIBUTHREAD_DIR) -luthread

# uthread_hello: link the object files and lib
$(UTHREAD_HELLO): $(UH_OBJ) $(LIBUTHREAD_DIR)/libuthread.a
	$(CC) $(CFLAGS) -o $@ $< -L$(LIBUTHREAD_DIR) -luthread

# compile the apps/*.c to object files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@ 

# build the library first
$(LIBUTHREAD_DIR)/libuthread.a:
	$(MAKE) -C $(LIBUTHREAD_DIR) DEBUG=$(DEBUG)

clean:
	$(MAKE) -C $(LIBUTHREAD_DIR) clean
	rm -f *.o $(TARGET) $(QUEUE_TESTER) $(UTHREAD_HELLO)

# run: $(TARGET)
# 	$(TARGET)

qt: $(QUEUE_TESTER)
	$(QUEUE_TESTER)

uh: $(UTHREAD_HELLO)
	$(UTHREAD_HELLO)

debug: DEBUG = 1
debug: CFLAGS = $(DEBUG_CFLAGS)
debug: clean all run
	@echo "Built the debug version"

.PHONY: all clean qt debug