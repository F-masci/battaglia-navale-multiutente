#include "../lib/lib.h"
#include "../config/config.h"
#include "../config/cmd.h"
#include "handler.h"

bool wait_string(player_t *, char *);
bool write_string(player_t *, char *);

extern size_t n_players;                                // Numero di giocatori in lobby
extern player_t **players;                              // Array di giocatori

#define BUFF_LEN 1024
void *client_handler(void *args) {

    player_t *player = (player_t *) malloc(sizeof(*player));
    bzero(player, sizeof(*player));
    player->socket = *( (int *) args );
    player->ready = false;
    addPlayer(player);
    sprintf(player->nickname, "Giocatore %lu", player->index + 1);

    cmd_t cmd;

    char *buffer = (char *) malloc(sizeof(*buffer) * BUFF_LEN);

handler_loop:
        cmd = wait_cmd(player);
        if(cmd == CMD_ERROR) goto handler_exit;
        PRINT("%s: request command %hu\n", player->nickname, cmd)
        switch(cmd) {
            case CMD_SET_NICKNAME: 
                if(wait_string(player, buffer) == false) goto handler_exit;
                PRINT("%s: set nickname ", player->nickname)
                if(setNicknamePlayer(player->index, buffer)) PRINT("%s\n", player->nickname)
                break;

            case CMD_LIST_PLAYERS: 
                bzero(buffer, BUFF_LEN);
                for(size_t i=0; i<n_players; i++) {
                    strcat(buffer, players[i]->nickname);
                    strcat(buffer, ";");
                }
                write_string(player, buffer);
                break;

            case CMD_START_GAME:
                player->ready = true;
                for(size_t i=0; i<n_players; i++) {
                    if(players[i]->ready == false) goto handler_loop;
                }
                PRINT("Server: all players ready\n")
                for(size_t i=0; i<n_players; i++) {
                    send_cmd(players[i], CMD_START_GAME);
                }
                break;
            
            default: goto handler_exit;
        }
    goto handler_loop;

handler_exit:
    PRINT("%s: disconnected\n", player->nickname)
    send_cmd(player, CMD_CLOSE_CONNECTION);
    removePlayer(player->index);
    free(player);
    free(buffer);

    return NULL;

}

bool wait_string(player_t *player, char *buffer) {

    bzero(buffer, BUFF_LEN);
    if(read(player->socket, buffer, BUFF_LEN) <= 0) return false;
    return true;

}

bool write_string(player_t *player, char *buffer) {

    if(write(player->socket, buffer, strlen(buffer)) <= 0)  return false;
    return true;

}
#undef BUFF_LEN
