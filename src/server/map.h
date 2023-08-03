#pragma once

#include "../lib/lib.h"
#include "../config/config.h"

typedef char cell_t;

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

#include "player.h"