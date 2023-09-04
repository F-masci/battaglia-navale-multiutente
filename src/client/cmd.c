#include "cmd.h"

extern int socket_client;

cmd_t waitCmd(void) {
    
    cmd_t cmd;
    if(recv(socket_client, &cmd, sizeof(cmd), MSG_WAITALL) < (ssize_t) sizeof(cmd)) return CMD_ERROR;
    return cmd;

}

bool sendCmd(cmd_t cmd) {

    if(write(socket_client, &cmd, sizeof(cmd)) <= 0) return false;
    return true;

}