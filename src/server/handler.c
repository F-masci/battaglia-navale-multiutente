#include "../lib/lib.h"
#include "../config/config.h"
#include "../config/cmd.h"
#include "player.h"
#include "cmd.h"

bool wait_string(player_t *, char *);
bool write_string(player_t *, char *);

#define BUFF_LEN 1024
void *client_handler(void *args) {

    player_t *player = (player_t *) malloc(sizeof(*player));
    bzero(player, sizeof(*player));
    player->socket = *( (int *) args );
    addPlayer(player);

    bool hasNick = false;   // Indica che non è stato ancora impostato un nickname
    cmd_t cmd;

    char *buffer = (char *) malloc(sizeof(*buffer) * BUFF_LEN);

handler_loop:
        cmd = wait_cmd(player);
        if(cmd == CMD_ERROR) goto handler_exit;
        PRINT("%s: request command %hu\n", hasNick ? player->nickname : "Cliente", cmd)
        switch(cmd) {
            case CMD_SET_NICKNAME: 
                if(wait_string(player, buffer) == false) goto handler_exit;
                if(setNicknamePlayer(player->index, buffer)) {
                    hasNick = true;
                    PRINT("Client: set nickname %s\n", player->nickname)
                }
                break;
            
            default: goto handler_exit;
        }
    goto handler_loop;

handler_exit:
    send_cmd(player, CMD_CLOSE_CONNECTION);
    removePlayer(player->index);
    free(player);
    free(buffer);

    return NULL;

}

bool wait_string(player_t *player, char *buffer) {

    bzero(buffer, BUFF_LEN);
    if(read(player->socket, buffer, BUFF_LEN) <= 0)  return false;
    return true;

}

bool write_string(player_t *player, char *buffer) {

    if(write(player->socket, buffer, strlen(buffer)) <= 0)  return false;
    return true;

}
#undef BUFF_LEN
