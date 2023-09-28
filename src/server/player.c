#include "player.h"

extern player_t **players;                              // Array di giocatori
static size_t players_len = DEFAULT_PLAYERS_LEN;        // Lunghezza dell'array dei giocatori
extern uint8_t n_players;                               // Numero di giocatori in lobby
extern int semid;                                       // Semaforo per sincronizzare la ricezione delle mappe

/**
 * @brief Crea un nuovo giocatore
 * 
 * @param[in] _fd_socket Descrittore del socket 
 * @return Puntatore alla struttura del giocatore
 * @retval ```NULL``` in caso di errore - viene impostato errno
 */
player_t *createPlayer(const int _fd_socket) {
    errno = 0;
    player_t *player = (player_t *) malloc(sizeof(*player));
    if(player != NULL) {
        BZERO(player, sizeof(*player));
        player->socket = _fd_socket;
        player->ready = false;
        if(!initPlayerMap(player)) return NULL;
    }
    return player;
}

/**
 * @brief Inizializza l'array dei giocatori
 * 
 * @return L'indirizzo di memoria allocata
 * @retval ```NULL``` in caso di errore - viene impostato errno
 */
player_t **initPlayersArray(void) {
    errno = 0;
    players = (player_t **) malloc(sizeof(*players) * players_len);
    if(players != NULL) BZERO(players, sizeof(*players) * players_len);
    return players;
}

/**
 * @brief Aggiugne un giocatore all'array dei giocatori
 * 
 * @param[in] _player Giocatore da inserire
 * @return Ritorna se l'azione è stata eseguita correttamente o meno
 * 
 * @retval ```true``` Giocatore inserito correttamente
 * @retval ```false``` Il giocatore non è stato inserito - viene impostato errno
 * 
 * @note In caso di fallimento l'array di giocatori non viene modificato e il giocatore non viene aggiunto
 */
bool addPlayer(player_t * const _player) {
    errno = 0;
    if(n_players == players_len) {    // Devo aumentare la dimensione dell'array - raddoppio ogni volta la dimensione
        player_t **aux = players;
        players_len *= 2;
        players = reallocarray(players, players_len, sizeof(*players));
        if(players == NULL) {
            players = aux;
            return false;
        }
    }
    _player->index = n_players;
    players[n_players++] = _player;
    return true;
}

/**
 * @brief Rimuove un giocatore all'array dei giocatori
 * 
 * @note La struttura dati del giocatore viene deallocata e il socket chiuso
 * 
 * @param[in] _index Giocatore da inserire
 * @return Ritorna se l'azione è stata eseguita correttamente o meno
 * 
 * @retval ```true``` Giocatore rimosso correttamente
 * @retval ```false``` Il giocatore non è stato rimosso - viene impostato errno
 */
bool removePlayer(const size_t _index) {
    errno = 0;

    player_t *player = players[_index];
    while(close(player->socket) == -1) {
        if(errno != EINTR) return false;
        else errno = 0;
    };

    for(uint8_t i=0; i<MAP_SIZE; i++) {
        free(player->map->grid[i]);
    }
    free(player->map->grid);
    free(player->map->ships);
    free(player->map);
    free(player);

    for(uint8_t i = _index; i<n_players-1; i++) {
        players[i] = players[i+1];
        players[i]->index = i;
    }
    players[n_players-1] = 0;
    n_players--;

    if(semid != -1) {

        struct sembuf so;
        BZERO(&so, sizeof(so));
        so.sem_num = 0;
        so.sem_op = (short) 1;
        so.sem_flg = 0;
        while(semop(semid, &so, 1) == -1) {
            EXIT_ERRNO
            errno = 0;
        };

    }

    return true;
}

/**
 * @brief Imposta il nickname di un giocatore
 * 
 * @param[in] _index Giocatore
 * @param[in] _nickname Nickname da impostare
 * @return Ritorna se l'azione è stata eseguita correttamente o meno
 * 
 * @retval ```true``` Nickname impostato correttamente
 * @retval ```false``` Il nickname non è stato impostato
 */
bool setNicknamePlayer(const size_t _index, const char * const _nickname) {
    size_t len = strlen(_nickname);
    if(len > NICKNAME_LEN-1) return false;
    BZERO(players[_index]->nickname, NICKNAME_LEN);
    memcpy(players[_index]->nickname, _nickname, len);
    return true;
}