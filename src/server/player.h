#pragma once

#include "type.h"

#include "../shared/lib.h"
#include "../shared/config.h"
#include "map.h"

player_t *createPlayer(int);
void *initPlayersArray(void);
bool addPlayer(player_t *);
bool removePlayer(size_t);
bool setNicknamePlayer(size_t, char *);