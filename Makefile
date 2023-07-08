CC=gcc
CFLAGS=$(FLAGS) -Wall -Wextra -g -lpthread
TARGET=main
SRC_DIR=src
OBJ_DIR=obj

SRCS=$(wildcard $(SRC_DIR)/*.c)
OBJS=$(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRCS))

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $< -c $(CFLAGS) -o $@


all: $(OBJS)
	$(CC) $(OBJS) $(CFLAGS) -o $(TARGET)

.PHONY: clean
clean:
	-$(RM) $(TARGET) $(OBJS)
