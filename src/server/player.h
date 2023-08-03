#pragma once

#include "type.h"

#include "../lib/lib.h"
#include "../config/config.h"
#include "map.h"

player_t *createPlayer(int);
void *initPlayersArray(void);
bool addPlayer(player_t *);
bool removePlayer(size_t);
bool setNicknamePlayer(size_t, char *);