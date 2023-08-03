#pragma once

#include "../lib/lib.h"
#include "../config/config.h"
#include "map.h"

typedef struct _player_t {
    size_t index;   // Contiene l'indice della struttura nell'array per una ricerca più rapida
    int socket;
    char nickname[NICKNAME_LEN];
    bool ready;
    map_t *map;
} player_t;

extern void initPlayerMap(player_t *);