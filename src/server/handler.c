#include "handler.h"

extern uint8_t n_players;                               // Numero di giocatori in lobby
extern player_t **players;                              // Array di puntatori ai metadati dei giocatori

extern pthread_t *w_threads;                            // Waiting threads
extern int semid;                                       // Semaforo per sincronizzare la ricezione delle mappe

static bool started = false;

void *clientHandler(void *_ptr) {

    errno = 0;

    int socket = *( (int *) _ptr);
    player_t *player = createPlayer(socket);
    if(player == NULL) {
        CHECK_ERRNO("Error")
        return NULL;
    }

    if(!addPlayer(player)) {
        CHECK_ERRNO("Error")
        return NULL;
    }

    if(sprintf(player->nickname, "Giocatore %lu", player->index + 1) <= -1) {
        CHECK_ERRNO("Error")
        return NULL;
    };

    cmd_t cmd;                  // Contiene il comando selezionato dal giocatore
    char *buffer = NULL;        // Buffer di appoggio per la lettura/ricezione di stringhe

handler_loop:
        PRINT("[%s]: waiting command\n", player->nickname)
        cmd = waitCmd(player);
        PRINT("[%s]: request command %hhu\n", player->nickname, cmd)
        if(cmd == CMD_ERROR) goto handler_exit;
        switch(cmd) {

            case CMD_SET_NICKNAME: 
                if(!waitString(player, &buffer)) {
                    CHECK_ERRNO("Error")
                    goto handler_exit;
                }
                PRINT("[%s]: set nickname ", player->nickname)
                if(setNicknamePlayer(player->index, buffer)) {
                    PRINT("%s\n", player->nickname)
                } else {
                    CHECK_ERRNO("Error")
                    goto handler_exit;
                }
                free(buffer);
                buffer = NULL;
                break;

            case CMD_LIST_PLAYERS: 
                buffer = (char *) malloc(sizeof(*buffer) * (n_players * NICKNAME_LEN + 1));
                BZERO(buffer, sizeof(*buffer) * n_players * NICKNAME_LEN);
                DEBUG("[DEBUG]: allocated %ld bytes for players list\n", sizeof(*buffer) * (n_players * NICKNAME_LEN + 1))
                for(uint8_t i=0; i<n_players; i++) {
                    strcat(buffer, players[i]->nickname);
                    strcat(buffer, ";");
                }
                if(!writeString(player, buffer)) {
                    CHECK_ERRNO("Error")
                    goto handler_exit;
                };
                free(buffer);
                buffer = NULL;
                break;

            case CMD_START_GAME:
                player->ready = true;

                if(n_players > 1) {
                    for(uint8_t i=0; i<n_players; i++) {
                        if(players[i]->ready == false) goto handler_loop;
                    }

                    PRINT("[SERVER]: all players ready\n")
                    started = true;
                    for(size_t i=0; i<WAITING_THREADS; i++) {
                        if(pthread_kill(w_threads[i], SIGUSR1) != 0) EXIT_ERRNO
                    }
                    free(w_threads);
                    w_threads = NULL;
                    if(kill(getpid(), SIGUSR2) == -1) EXIT_ERRNO

                    for(size_t i=0; i<n_players; i++) {
                        if(!sendCmd(players[i], CMD_START_GAME)) EXIT_ERRNO
                    }
                }
                break;

            case CMD_SEND_MAP:
                if(!waitString(player, &buffer)) {
                    CHECK_ERRNO("Error")
                    goto handler_exit;
                }
                char *cur = buffer;
                for(uint8_t k=0; k<SHIPS_NUM; k++) {
                    player->map->ships[k].dim = (uint8_t) *cur++ - '0';
                    player->map->ships[k].x = (uint8_t) *cur++ - '0';
                    player->map->ships[k].y = (uint8_t) *cur++ - '0';
                    player->map->ships[k].dir = *cur++;
                    player->map->ships[k].sunk = false;
                }

                makeMap(player);
                PRINT("[%s]: Ships recived and map created\n", player->nickname);

                // Utilizzo il semaforo per notificare al server che la mappa relativa a questo giocatore è stata ricevuta

                struct sembuf so;
                BZERO(&so, sizeof(so));
                so.sem_num = 0;
                so.sem_op = 1;
                so.sem_flg = 0;
                while(semop(semid, &so, 1) == -1) EXIT_ERRNO

                free(buffer);
                buffer = NULL;
                pthread_exit(NULL);
                break;
            
            case CMD_CLOSE_CONNECTION: goto handler_exit;

            default: goto handler_exit;
        }
    goto handler_loop;

handler_exit:

    errno = 0;

    PRINT("[%s]: disconnected\n", player->nickname)
    if(!sendCmd(player, CMD_CLOSE_CONNECTION)) if(errno != EPIPE) EXIT_ERRNO
    if(!removePlayer(player->index)) EXIT_ERRNO
    if(buffer != NULL) free(buffer);

    if(!started && n_players > 1) {

        for(uint8_t i=0; i<n_players; i++) {
            if(players[i]->ready == false) return NULL;
        }

        PRINT("[SERVER]: all players ready\n")
        for(size_t i=0; i<WAITING_THREADS; i++) {
            if(pthread_kill(w_threads[i], SIGUSR1) != 0) EXIT_ERRNO
        }
        free(w_threads);
        w_threads = NULL;
        if(kill(getpid(), SIGUSR2) == -1) EXIT_ERRNO

        for(size_t i=0; i<n_players; i++) {
            if(!sendCmd(players[i], CMD_START_GAME)) EXIT_ERRNO
        }
    }

    return NULL;

}