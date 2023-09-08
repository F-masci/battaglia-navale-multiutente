#pragma once

#include "../shared/lib.h"
#include "../shared/config.h"
#include "helpers.h"
#include "cmd.h"
#include "player.h"

void send_maps(player_t *player, size_t index);
void gameInitialization(void);
void send_map(player_t *player);
void get_move(player_t *player, int ind);