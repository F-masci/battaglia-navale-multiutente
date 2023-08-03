#include "handler.h"
#include "map.h"

extern size_t n_players;                                // Numero di giocatori in lobby
extern player_t **players;                              // Array di giocatori

extern pthread_t *wthreads;                             // Waiting threads

#define BUFF_LEN 1024
void *client_handler(void *socket_addr) {

    player_t *player = createPlayer( *( (int *) socket_addr) );
    addPlayer(player);
    sprintf(player->nickname, "Giocatore %lu", player->index + 1);

    cmd_t cmd;

    char *buffer = (char *) malloc(sizeof(*buffer) * BUFF_LEN);
    char *map_encoded=(char *)malloc(sizeof(*map_encoded)*MAP_SIZE*MAP_SIZE);
    int p=0;

handler_loop:
        cmd = wait_cmd(player);
        if(cmd == CMD_ERROR) goto handler_exit;
        PRINT("[%s]: request command %hu\n", player->nickname, cmd)
        switch(cmd) {
            case CMD_SET_NICKNAME: 
                if(wait_string(player, buffer) == false) goto handler_exit;
                PRINT("[%s]: set nickname ", player->nickname)
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
                PRINT("[SERVER]: all players ready\n")
                for(size_t i=0; i<WAITING_THREADS; i++) {
                    pthread_kill(wthreads[i], SIGUSR1);
                }
                for(size_t i=0; i<n_players; i++) {
                    send_cmd(players[i], CMD_START_GAME);
                }
                break;

            case CMD_SEND_MAP:
                wait_string(player, map_encoded);
                initPlayerMap(player);
                for(int i=0; i<MAP_SIZE; i++){
                    for(int j=0; j<MAP_SIZE; j++){
                        player->map->grid[i][j]=map_encoded[p];
                        p++;
                    }
                }
                pthread_exit(NULL);
                break;
            
            default: goto handler_exit;
        }
    goto handler_loop;

handler_exit:
    PRINT("[%s]: disconnected\n", player->nickname)
    send_cmd(player, CMD_CLOSE_CONNECTION);
    removePlayer(player->index);
    free(player);
    free(buffer);

    return NULL;

}
#undef BUFF_LEN