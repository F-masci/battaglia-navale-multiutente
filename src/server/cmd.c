#include "../lib/lib.h"
#include "../config/cmd.h"
#include "player.h"

#define BUFF_LEN 3 // Massimo numero di cifre di un cmd_t
cmd_t wait_cmd(player_t *player) {

    char *buffer = (char *)malloc(sizeof(*buffer) * BUFF_LEN);
    bzero(buffer, strlen(buffer));
    
    if(read(player->socket, buffer, BUFF_LEN) <= 0) {
        removePlayer(player->index);
        free(player);
        free(buffer);

        return CMD_ERROR;
    }
    free(buffer);
    return (cmd_t) strtoul(buffer, NULL, 10);

}

bool send_cmd(player_t *player, cmd_t cmd) {

    char *buffer = (char *)malloc(sizeof(*buffer) * BUFF_LEN);
    bzero(buffer, strlen(buffer));

    sprintf(buffer, "%hu", cmd);

    if(write(player->socket, buffer, strlen(buffer)) <= 0) {
        removePlayer(player->index);
        free(player);
        free(buffer);

        return false;
    }
    free(buffer);
    return true;
            
}
#undef BUFF_LEN