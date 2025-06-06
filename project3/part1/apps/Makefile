LIB_DIR = ./libfs
LIB_NAME = fs
LIB = libfs.o
CC = gcc
CFLAGS = -Wall -Wextra -I$(LIB_DIR) -Werror
DEBUG_CFLAGS = -Wall -Wextra -I$(LIB_DIR) -fsanitize=address -g
DEBUG = 0
SRC_DIR = .
BUILD_DIR = .

APP_TESTER = $(BUILD_DIR)/app_test
APP_SRC = $(SRC_DIR)/test_fs.c
APP_OBJ = $(BUILD_DIR)/test_fs.o

all: $(APP_TESTER)

# app_test: link the object files and lib
$(APP_TESTER): $(APP_OBJ) $(LIB_DIR)/$(LIB)
	$(CC) $(CFLAGS) -o $@ $< -L$(LIB_DIR) -l$(LIB_NAME)

# compile the apps/APP_SRC to object files
$(APP_OBJ): $(APP_SRC)
	$(CC) $(CFLAGS) -c $< -o $@

# build the library first
$(LIB_DIR)/$(LIB):
	$(MAKE) -C $(LIB_DIR) DEBUG=$(DEBUG)

clean:
	$(MAKE) -C $(LIB_DIR) clean
	rm -f *.o $(TARGET) $(APP_TESTER)

run: $(APP_TESTER)
	$(APP_TESTER)

test: $(APP_TESTER)
	$(APP_TESTER)

debug: DEBUG = 1
debug: CFLAGS = $(DEBUG_CFLAGS)
debug: clean all
	@echo "Built the debug version"

.PHONY: all clean debug test