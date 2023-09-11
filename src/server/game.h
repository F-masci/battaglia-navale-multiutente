#pragma once

#include "../shared/lib.h"
#include "../shared/config.h"
#include "helpers.h"
#include "cmd.h"
#include "player.h"

void gameInitialization(void);
bool sendMaps(const player_t * const player);
bool sendMap(const player_t * const player);
void getMove(player_t *player);