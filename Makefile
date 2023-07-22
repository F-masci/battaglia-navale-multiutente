CC=gcc
CFLAGS=$(FLAGS) -Wall -Wextra -g -lpthread
TARGET=main
SRC_DIR=src
OBJ_DIR=obj

SRV_DIR=server
CLN_DIR=client

$(OBJ_DIR)/$(SRV_DIR)/%.o: $(SRC_DIR)/$(SRV_DIR)/%.c
	$(CC) $< -c $(CFLAGS) -o $@

$(OBJ_DIR)/$(CLN_DIR)/%.o: $(SRC_DIR)/$(CLN_DIR)/%.c
	$(CC) $< -c $(CFLAGS) -o $@

all:
	echo "Compile with make server or make client"

server: $(patsubst $(SRC_DIR)/$(SRV_DIR)/%.c, $(OBJ_DIR)/$(SRV_DIR)/%.o, $(wildcard $(SRC_DIR)/$(SRV_DIR)/*.c))
	$(CC) $(patsubst $(SRC_DIR)/$(SRV_DIR)/%.c, $(OBJ_DIR)/$(SRV_DIR)/%.o, $(wildcard $(SRC_DIR)/$(SRV_DIR)/*.c)) -o server $(CFLAGS)

client: $(patsubst $(SRC_DIR)/$(CLN_DIR)/%.c, $(OBJ_DIR)/$(CLN_DIR)/%.o, $(wildcard $(SRC_DIR)/$(CLN_DIR)/*.c))
	$(CC) $(patsubst $(SRC_DIR)/$(CLN_DIR)/%.c, $(OBJ_DIR)/$(CLN_DIR)/%.o, $(wildcard $(SRC_DIR)/$(CLN_DIR)/*.c)) -o client $(CFLAGS)

.PHONY: clean
clean:
	-$(RM) $(TARGET) $(OBJ_DIR)/*/*
