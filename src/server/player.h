#pragma once

#include "../lib/lib.h"
#include "../config/config.h"

typedef struct _player_t {
    size_t index;   // Contiene l'indice della struttura nell'array per una ricerca più rapida
    int socket;
    char nickname[NICKNAME_LEN];
} player_t;

void *initPlayersArray(void);
bool addPlayer(player_t *);
bool removePlayer(size_t);
bool setNicknamePlayer(size_t, char *);