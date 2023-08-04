#include "cmd.h"

cmd_t waitCmd(player_t *player) {
    
    cmd_t cmd;
    if(read(player->socket, &cmd, sizeof(cmd)) <= 0) return CMD_ERROR;
    return cmd;

}

bool sendCmd(player_t *player, cmd_t cmd) {

    if(write(player->socket, &cmd, sizeof(cmd)) <= 0) return false;
    return true;

}