#include "game.h"

extern uint8_t n_players;
extern player_t **players;

static char *_make_message(const char * const _format, ...);

/**
 * @brief Invia ai client i dati dei giocatori connessi alla partita
 * 
 */
void gameInitialization(void){

    size_t nick_len;
    char *encoded = (char *) malloc((n_players * NICKNAME_LEN + 1) * sizeof(*encoded));
    if(encoded == NULL) EXIT_ERRNO
    BZERO(encoded, (n_players * NICKNAME_LEN + 1) * sizeof(*encoded));
    DEBUG("[DEBUG]: allocated %lu for game init\n", (n_players * NICKNAME_LEN + 1) * sizeof(*encoded))

    char *cur = encoded;

    for(uint8_t i=0; i<n_players; i++){
        nick_len = strlen(players[i]->nickname);
        memcpy(cur, players[i]->nickname, nick_len);
        cur += nick_len;
        *cur++=';';
    }

    for(uint8_t i=0; i<n_players; i++){
        if(!writeNum(players[i], n_players)) EXIT_ERRNO
        if(!writeNum(players[i], i)) EXIT_ERRNO
        if(!writeString(players[i], encoded)) EXIT_ERRNO
    }
    
}

/**
 * @brief Calcola la mossa di un giocatore
 * 
 * @param _player Giocatore che sta eseguendo la mossa
 * @return Giocatore eliminato
 * 
 * @retval Un numero positivo diverso dall'indice di ```_player``` indica l'indice del giocatore eliminato
 * @retval ```-1``` in caso di errore - viene impostato errno
 */
int16_t getMove(const player_t * const _player) {

    uint8_t index, x1, y1;
    int i;

    char *message = NULL;
    char *encoded_move = NULL;

    if(!waitString(_player, &encoded_move)) return -1;

    index = encoded_move[0] - '0';
    x1 = encoded_move[1] - '0';
    y1 = encoded_move[2] - '0';
    free(encoded_move);

    if(players[index]->map->grid[y1][x1] == '1'){
        players[index]->map->grid[y1][x1] = '2';
        message = _make_message("%s ha colpito %s [%c %hhu]", _player->nickname, players[index]->nickname, 'A' + x1, y1);
        if(message == NULL) return -1;
    }
    else if(players[index]->map->grid[y1][x1] == '0' || players[index]->map->grid[y1][x1] == '3'){
        players[index]->map->grid[y1][x1] = '3';
        message = _make_message("%s ha mancato %s [%c %hhu]", _player->nickname, players[index]->nickname,'A' + x1, y1);
        if(message == NULL) return -1;
    }
    else if(players[index]->map->grid[y1][x1] == '2'){
        message = _make_message("%s ha colpito una parte di una nave di %s già colpita [%c %hhu]", _player->nickname, players[index]->nickname, 'A' + x1, y1);
        if(message == NULL) return -1;
    }

    PRINT("[%s]: %s", _player->nickname, message)

    for(uint8_t h=0; h<n_players; h++){
        if(!sendCmd(players[h], CMD_STATUS)) return -1;
        if(!writeString(players[h], message)) return -1;
    }

    free(message);
    message = NULL;

    //il server esegue il controllo 
    uint8_t x, y, dim;
    
    //aggiorna 
    for(uint8_t k=0; k<SHIPS_NUM; k++){
        if(players[index]->map->ships[k].sunk == false) {
            
            x = players[index]->map->ships[k].x;
            y = players[index]->map->ships[k].y;
            dim = players[index]->map->ships[k].dim;

            switch(players[index]->map->ships[k].dir){
                case 'W':
                    for(i=y; i>y-dim; i--){
                        if(players[index]->map->grid[i][x] =='1') goto next;
                    }
                    players[index]->map->ships[k].sunk = true;
                    break;
                case 'A':
                    for(i=x; i>x-dim; i--){
                        if(players[index]->map->grid[y][i] =='1') goto next;
                    }
                    players[index]->map->ships[k].sunk = true;
                    break;
                case 'S':
                    for(i=y; i<y+dim; i++){
                        if(players[index]->map->grid[i][x] =='1') goto next;
                    }
                    players[index]->map->ships[k].sunk = true;
                    break;
                case 'D':
                    for(i=x; i<x+dim; i++){
                        if(players[index]->map->grid[y][i] =='1') goto next;
                    }
                    players[index]->map->ships[k].sunk = true;
                    break;
                default: break;
            }
        } else {
            goto next;
        }

        switch(dim) {
            case 2:
                message = _make_message("%s ha affondato CACCIATORPEDINIERE di %s\n", _player->nickname, players[index]->nickname);
                if(message == NULL) return -1;
                break;
            case 3:
                message = _make_message("%s ha affondato SOTTOMARINO di %s\n", _player->nickname, players[index]->nickname);
                if(message == NULL) return -1;
                break;
            case 4:
                message = _make_message("%s ha affondato CORAZZATA di %s\n", _player->nickname, players[index]->nickname);
                if(message == NULL) return -1;
                break;
            case 5:
                message = _make_message("%s ha affondato PORTAEREI di %s\n", _player->nickname, players[index]->nickname);
                if(message == NULL) return -1;
                break;
            default: break;
        }

        goto send;

next:
        ;
    }

    message = _make_message("%s non ha affondato nessuna nave di %s\n", _player->nickname, players[index]->nickname);
    if(message == NULL) return -1;

send:

    PRINT("[%s]: %s", _player->nickname, message)
    if(message == NULL) return -1;

    for(uint8_t h=0; h<n_players; h++){
        if(!writeString(players[h], message)) return -1;
    }

    free(message);
    message = NULL;

    bool elim = true;

    for(uint8_t k=0; k<SHIPS_NUM; k++){
        if(players[index]->map->ships[k].sunk == false){
            elim = false;
            break;
        }
    }

    if(elim) return (int16_t) index;

    return (int16_t) _player->index;
}

/**
 * @brief Invia tutte le mappe al giocatore selezionato
 * 
 * @param[in] _player Giocatore a cui inviare le mappe 
 * @return Ritorna se l'azione è stata eseguita correttamente o meno
 * 
 * @retval ```true``` Mappe inviate correttamente
 * @retval ```false``` La mappe non sono state inviate - viene impostato errno
 */
bool sendMaps(const player_t * const _player) {
    
    errno = 0;
    uint8_t index = _player->index;

    // N_PLAYERS * MAP_SIZE * MAP_SIZE => mappa di ogni giocatore
    // +
    // N_PLAYERS => prefisso di ogni mappa con indice giocatore
    // +
    // 1 => terminatore di stringa
    size_t len = (n_players * MAP_SIZE * MAP_SIZE) + n_players + 1;
    char *encoded = (char *) malloc(len * sizeof(*encoded));
    if(encoded == NULL) return false;
    BZERO(encoded, len * sizeof(*encoded));

    DEBUG("[DEBUG]: allocated %ld bytes\n", len * sizeof(*encoded))

    for(uint8_t i=0; i<n_players; i++){
        
        *encoded++ = '0' + (i+1);

        for(uint8_t j=0; j<MAP_SIZE; j++){
            memcpy(encoded, players[i]->map->grid[j], MAP_SIZE * sizeof(*encoded));
            encoded += MAP_SIZE;
        }
    }

    encoded -= (n_players * MAP_SIZE * MAP_SIZE) + n_players;

    DEBUG("[DEBUG]: sending %s (%ld chars)\n", encoded, strlen(encoded))

    if(!writeString(_player, encoded)) {
        free(encoded);
        return false;
    }

    free(encoded);
    return true;

}

/**
 * @brief Invia una mappa al giocatore selezionato
 * 
 * @param[in] _player Giocatore a cui inviare la mapp 
 * @return Ritorna se l'azione è stata eseguita correttamente o meno
 * 
 * @retval ```true``` Mappa inviata correttamente
 * @retval ```false``` La mappa non è stata inviata - viene impostato errno
 */
bool sendMap(const player_t * const _player) {

    errno = 0;

    // MAP_SIZE * MAP_SIZE => mappa del giocatore
    // +
    // 1 => terminatore di stringa
    size_t len = (MAP_SIZE * MAP_SIZE + 1);
    char *encoded = (char *) malloc(len * sizeof(*encoded));
    if(encoded == NULL) return false;
    BZERO(encoded, len * sizeof(*encoded));

    DEBUG("[DEBUG]: allocated %ld bytes\n", len * sizeof(*encoded))
    
    uint8_t i;
    if(!waitNum(_player, (uint32_t *) &i)) {
        free(encoded);
        return false;
    }

    DEBUG("[DEBUG]: sending map of %s to %s\n", players[i]->nickname, _player->nickname)

    for(int j=0; j<MAP_SIZE; j++){
        for(int k=0; k<MAP_SIZE; k++){
            *encoded++ = players[i]->map->grid[j][k];
        }
    }
    encoded -= MAP_SIZE * MAP_SIZE;

    DEBUG("[DEBUG]: sending %s (%ld chars)\n", encoded, strlen(encoded))

    if(!writeString(_player, encoded)) {
        free(encoded);
        return false;
    };
    free(encoded);
    return true;

}

/**
 * @brief Compone il messaggio da inviare ai client
 * 
 * @param[in] _format Stringa di formato del messaggio
 * @param[in] ... Argomenti della stringa di formato
 * @return Buffer contenente il messaggio
 * 
 * @retval ```NULL``` in caso di errore
 */
static char *_make_message(const char * const _format, ...) {

    errno = 0;

    size_t size = 0;
    char *msg = NULL;
    va_list args_list;

    /* -- MSG LEN -- */

    va_start(args_list, _format);
    int msg_len = vsnprintf(NULL, 0, _format, args_list);
    va_end(args_list);
    if(msg_len <= -1) return NULL;
    size = (size_t) msg_len + 1;
    msg = malloc(sizeof(*msg) * size);
    if(msg == NULL) return NULL;
    BZERO(msg, size);

    /* -- MSG -- */

    va_start(args_list, _format);
    int check = vsnprintf(msg, size, _format, args_list);
    va_end(args_list);

    if (check <= -1) {
        free(msg);
        return NULL;
    }

    return msg;
}