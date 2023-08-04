#include "cmd.h"

extern int socket_client;

cmd_t waitCmd() {
    
    cmd_t cmd;
    if(read(socket_client, &cmd, sizeof(cmd)) <= 0) return CMD_ERROR;
    return cmd;

}

bool sendCmd(cmd_t cmd) {

    if(write(socket_client, &cmd, sizeof(cmd)) <= 0) return false;
    return true;

}