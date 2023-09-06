#pragma once
typedef uint8_t cmd_t;

#define CMD_ERROR 0
#define CMD_CLOSE_CONNECTION 1

#define CMD_SET_NICKNAME 2
#define CMD_LIST_PLAYERS 3
#define CMD_START_GAME 4
#define CMD_SEND_MAP 5

#define CMD_TURN 6
#define CMD_GET_MAPS 7
#define CMD_GET_MAP 8
#define CMD_MOVE 9
#define CMD_STATUS 10