#pragma once

#include "type.h"

#include "../shared/lib.h"
#include "../shared/config.h"
#include "player.h"

void initPlayerMap(player_t *);
void printMap(cell_t **map);
void makeMap(player_t *);
