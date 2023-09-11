CC=gcc
CFLAGS=$(FLAGS) -Wall -Wextra -g -lpthread -lm
TARGET=main
SRC_DIR=src
OBJ_DIR=obj
PROD_DIR=prod

SRV_DIR=server
CLN_DIR=client

SRC_SRV_DIR=$(SRC_DIR)/$(SRV_DIR)
SRC_CLN_DIR=$(SRC_DIR)/$(CLN_DIR)

OBJ_SRV_DIR=$(OBJ_DIR)/$(SRV_DIR)
OBJ_CLN_DIR=$(OBJ_DIR)/$(CLN_DIR)

OBJS_SRV=$(patsubst $(SRC_SRV_DIR)/%.c, $(OBJ_SRV_DIR)/%.o, $(wildcard $(SRC_SRV_DIR)/*.c))
OBJS_CLN=$(patsubst $(SRC_CLN_DIR)/%.c, $(OBJ_CLN_DIR)/%.o, $(wildcard $(SRC_CLN_DIR)/*.c))

OBJ_PROD_SRV=$(patsubst $(SRC_SRV_DIR)/%.c, $(PROD_DIR)/$(SRV_DIR)/%.o, $(wildcard $(SRC_SRV_DIR)/*.c))
OBJ_PROD_CLN=$(patsubst $(SRC_CLN_DIR)/%.c, $(PROD_DIR)/$(CLN_DIR)/%.o, $(wildcard $(SRC_CLN_DIR)/*.c))

$(OBJ_SRV_DIR)/%.o: $(SRC_SRV_DIR)/%.c
	$(CC) $< -c $(CFLAGS) -o $@

$(OBJ_CLN_DIR)/%.o: $(SRC_CLN_DIR)/%.c
	$(CC) $< -c $(CFLAGS) -o $@

all:
	make server
	make client

$(PROD_DIR)/$(SRV_DIR)/%.o: $(SRC_SRV_DIR)/%.c
	$(CC) $< -c $(CFLAGS) -o $@ -DPROD

$(PROD_DIR)/$(CLN_DIR)/%.o: $(SRC_CLN_DIR)/%.c
	$(CC) $< -c $(CFLAGS) -o $@ -DPROD

production: $(sort $(OBJ_PROD_SRV) $(OBJ_PROD_CLN))
	$(CC) $(OBJ_PROD_SRV) -o server $(CFLAGS) -DPROD
	$(CC) $(OBJ_PROD_CLN) -o client $(CFLAGS) -DPROD

.PHONY: init server client clean remake

init:
	mkdir -p ./$(OBJ_SRV_DIR)
	mkdir -p ./$(OBJ_CLN_DIR)
	mkdir -p ./$(PROD_DIR)/$(SRV_DIR)
	mkdir -p ./$(PROD_DIR)/$(CLN_DIR)

server: $(OBJS_SRV)
	$(CC) $(OBJS_SRV) -o server $(CFLAGS)

client: $(OBJS_CLN)
	$(CC) $(OBJS_CLN) -o client $(CFLAGS)

clean:
	-$(RM) $(TARGET) $(OBJ_DIR)/*/*
	-$(RM) $(TARGET) $(PROD_DIR)/*/*

remake:
	make clean
	make server
	make client