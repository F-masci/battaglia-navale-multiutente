#include "handler.h"

extern size_t n_players;                                // Numero di giocatori in lobby
extern player_t **players;                              // Array di giocatori

extern pthread_t *w_threads;                             // Waiting threads

#define BUFF_LEN 1024
void *client_handler(void *socket_addr) {

    player_t *player = createPlayer( *( (int *) socket_addr) );
    addPlayer(player);
    sprintf(player->nickname, "Giocatore %lu", player->index + 1);

    cmd_t cmd;

    char *buffer = (char *) malloc(sizeof(*buffer) * BUFF_LEN);
    char *map_encoded=(char *)malloc(sizeof(*map_encoded)*MAP_SIZE*MAP_SIZE);
    char *ships_encoded=(char *)malloc(sizeof(ship_t)*SHIPS_NUM);
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
                    pthread_kill(w_threads[i], SIGUSR1);
                }
                for(size_t i=0; i<n_players; i++) {
                    send_cmd(players[i], CMD_START_GAME);
                }
                break;

            case CMD_SEND_MAP:
                wait_string(player, map_encoded);
                wait_string(player, ships_encoded);
                //initPlayerMap(player);
                for(int i=0; i<MAP_SIZE; i++){
                    for(int j=0; j<MAP_SIZE; j++){
                        player->map->grid[i][j]=map_encoded[p];
                        p++;
                    }
                }
                /*p=0;
                for(int k=0; k<SHIPS_NUM; k++){
                    player->map->ships[k].dim=ships_encoded[p];
                    player->map->ships[k].x=ships_encoded[++p];
                    player->map->ships[k].y=ships_encoded[++p];
                    player->map->ships[k].dir=ships_encoded[++p];
                    p++;
                }
                */
                PRINT("[%s]: sent map\n", player->nickname);
                print_map(player->map->grid);

                /*PRINT("\n");
                for(int i=0; i<SHIPS_NUM; i++){
                    PRINT("\t[%d] ", i);
                    switch(player->map->ships[i].dim){
                        case 2:
                            PRINT("DESTROYER (%d,%d)\n", player->map->ships[i].x, player->map->ships[i].x);
                            break;
                        case 3:
                            PRINT("SUBMARINE (%d,%d)\n", player->map->ships[i].x, player->map->ships[i].x);
                            break;
                        case 4:
                            PRINT("BATTLESHIP\n");
                            break;
                        case 5:
                            PRINT("CARRIER\n");
                            break;
                        default: break;
                    }
                }
                */
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