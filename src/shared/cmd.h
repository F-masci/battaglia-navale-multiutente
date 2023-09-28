#pragma once
typedef uint8_t cmd_t;

#define CMD_ERROR 0
#define CMD_CLOSE_CONNECTION 1
#define CMD_NULL 2

#define CMD_SET_NICKNAME 3
#define CMD_LIST_PLAYERS 4
#define CMD_START_GAME 5
#define CMD_SEND_MAP 6

#define CMD_TURN 7
#define CMD_GET_MAPS 8
#define CMD_GET_MAP 9
#define CMD_MOVE 10
#define CMD_STATUS 11

#define CMD_DELETE_PLAYER 12
#define CMD_WIN 13