CC=gcc
CFLAGS=$(FLAGS) -Wall -Wextra -g -lpthread
SRC_DIR=src
OBJ_DIR=obj

SRV_DIR=server
CLN_DIR=client

SRC_SRV_DIR=$(SRC_DIR)/$(SRV_DIR)
SRC_CLN_DIR=$(SRC_DIR)/$(CLN_DIR)

OBJ_SRV_DIR=$(OBJ_DIR)/$(SRV_DIR)
OBJ_CLN_DIR=$(OBJ_DIR)/$(CLN_DIR)

OBJS_SRV=$(patsubst $(SRC_SRV_DIR)/%.c, $(OBJ_SRV_DIR)/%.o, $(wildcard $(SRC_SRV_DIR)/*.c))
OBJS_CLN=$(patsubst $(SRC_CLN_DIR)/%.c, $(OBJ_CLN_DIR)/%.o, $(wildcard $(SRC_CLN_DIR)/*.c))

$(OBJ_SRV_DIR)/%.o: $(SRC_SRV_DIR)/%.c
	$(CC) $< -c $(CFLAGS) -o $@ $(D)

$(OBJ_CLN_DIR)/%.o: $(SRC_CLN_DIR)/%.c
	$(CC) $< -c $(CFLAGS) -o $@ $(D)

all: $(sort $(OBJS_SRV) $(OBJS_CLN))
	$(CC) $(OBJS_SRV) -o server $(CFLAGS) $(D)
	$(CC) $(OBJS_CLN) -o client $(CFLAGS) $(D)

.PHONY: init clean

init:
	if [ ! -d "./$(OBJ_SRV_DIR)" ]; then mkdir -p ./$(OBJ_SRV_DIR); fi
	if [ ! -d "./$(OBJ_CLN_DIR)" ]; then mkdir -p ./$(OBJ_CLN_DIR); fi
	@echo "Init completed"

clean:
	-$(RM) server client $(OBJ_DIR)/*/*