#pragma once

#include "../lib/lib.h"
#include "../config/config.h"

typedef struct _player_t {
    size_t index;   // Contiene l'indice della struttura nell'array per una ricerca più rapida
    int socket;
    char nickname[NICKNAME_LEN];
    bool ready;
} player_t;