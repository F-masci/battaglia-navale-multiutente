#pragma once

#include "../shared/lib.h"
#include "../shared/config.h"

typedef unsigned char cell_t;

typedef struct _ship_t{
    uint8_t dim;
    uint8_t x;
    uint8_t y;
    char dir;
} ship_t;

typedef struct _map_t {
    cell_t **grid;
    ship_t *ships;
} map_t;

typedef struct _player_t {
    size_t index;   // Contiene l'indice della struttura nell'array per una ricerca più rapida
    int socket;
    char nickname[NICKNAME_LEN];
    bool ready;
    map_t *map;
} player_t;