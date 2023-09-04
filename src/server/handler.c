#include "handler.h"

extern uint8_t n_players;                                // Numero di giocatori in lobby
extern player_t **players;                              // Array di giocatori

extern pthread_t *w_threads;                            // Waiting threads
extern int semid;

#define BUFF_LEN 1024
void *clientHandler(void *socket_addr) {

    player_t *player = createPlayer( *( (int *) socket_addr) );
    addPlayer(player);
    sprintf(player->nickname, "Giocatore %lu", player->index + 1);

    cmd_t cmd;

    char *buffer = NULL;

handler_loop:
        cmd = waitCmd(player);
        if(cmd == CMD_ERROR) goto handler_exit;
        PRINT("[%s]: request command %hhu\n", player->nickname, cmd)
        switch(cmd) {
            case CMD_SET_NICKNAME: 
                if(waitString(player, &buffer) == false) goto handler_exit;
                PRINT("[%s]: set nickname ", player->nickname)
                if(setNicknamePlayer(player->index, buffer)) PRINT("%s\n", player->nickname)
                free(buffer);
                break;

            case CMD_LIST_PLAYERS: 
                bzero(buffer, BUFF_LEN);
                for(size_t i=0; i<n_players; i++) {
                    strcat(buffer, players[i]->nickname);
                    strcat(buffer, ";");
                }
                writeString(player, buffer);
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
                kill(getpid(), SIGUSR2);

                for(size_t i=0; i<n_players; i++) {
                    sendCmd(players[i], CMD_START_GAME);
                }
                break;

            case CMD_SEND_MAP:
                waitString(player, &buffer);
                char *cur = buffer;
                for(int k=0; k<SHIPS_NUM; k++) {
                    player->map->ships[k].dim = (uint8_t) *cur++ - '0';
                    player->map->ships[k].x = (uint8_t) *cur++ - '0';
                    player->map->ships[k].y = (uint8_t) *cur++ - '0';
                    player->map->ships[k].dir = *cur++;
                }

                makeMap(player);
                PRINT("[%s]: Map received\n", player->nickname);

                struct sembuf so;
                bzero(&so, sizeof(so));
                so.sem_num = 0;
                so.sem_op = 1;
                so.sem_flg = 0;
                semop(semid, &so, 1);

                free(buffer);
                pthread_exit(NULL);
                break;
            
            default: goto handler_exit;
        }
    goto handler_loop;

handler_exit:
    PRINT("[%s]: disconnected\n", player->nickname)
    sendCmd(player, CMD_CLOSE_CONNECTION);
    removePlayer(player->index);
    free(player);
    free(buffer);

    return NULL;

}
#undef BUFF_LEN