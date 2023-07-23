#include "../lib/lib.h"
#include "../config/config.h"
#include "../config/cmd.h"
#include "player.h"

#define BUFF_LEN 4096
void *lobby_handler(void *args) {

    int socket_client = *( (int *) args );

    player_t *player = (player_t *) malloc(sizeof(*player));
    bzero(player, sizeof(*player));
    player->socket = socket_client;
    addPlayer(player);

    bool hasNick = false;   // Indica che non è stato ancora impostato un nickname

    char *buffer = (char *)malloc(sizeof(*buffer) * BUFF_LEN);
    cmd_t cmd;

lobby_loop:
        bzero(buffer, strlen(buffer));
        if(read(socket_client, buffer, BUFF_LEN) <= 0) goto lobby_exit;
        PRINT("%s: request command %s\n", hasNick ? player->nickname : "Cliente",buffer)
        cmd = (cmd_t) strtoul(buffer, NULL, 10);
        switch(cmd) {
            case SET_NICKNAME: 
                bzero(buffer, strlen(buffer));
                if(read(socket_client, buffer, BUFF_LEN) <= 0) goto lobby_exit;
                if(setNicknamePlayer(player->index, buffer)) {
                    hasNick = true;
                    PRINT("Client: set nickname %s\n", player->nickname)
                }
                break;
            
            default: goto lobby_exit;
        }
    goto lobby_loop;

lobby_exit:
    removePlayer(player->index);
    free(player);
    free(buffer);

    return NULL;

}
#undef BUFF_LEN