#include "../lib/lib.h"
#include "../config/config.h"
#include "player.h"

extern player_t **players;                              // Array di giocatori
static size_t players_len = DEFAULT_PLAYERS_LEN;        // Lunghezza dell'array dei giocatori
extern size_t n_players;                                // Numero di giocatori in lobby

void *initPlayersArray(void) {
    errno = 0;
    players = (player_t **) malloc(sizeof(*players) * players_len);
    return (void *) players;
}

bool addPlayer(player_t *player) {
    errno = 0;
    if(n_players == players_len) {    // Devo aumentare la dimensione dell'array - raddoppio ogni volta la dimensione
        players_len *= 2;
        players = realloc(players, players_len);
        if(players == NULL) return false;
    }
    player->index = n_players;
    players[n_players++] = player;
    return true;
}

bool removePlayer(size_t index) {
    errno = 0;
    close(players[index]->socket);
    for(size_t i = index; i<n_players-1; i++) {
        players[i] = players[i+1];
        players[i]->index = i;
    }
    players[n_players-1] = 0;
    n_players--;
    return true;
}

bool setNicknamePlayer(size_t index, char *nickname) {
    errno = 0;
    size_t len = strlen(nickname);
    if(len > 255) return false;
    memcpy(players[index]->nickname, nickname, len);
    return true;
}