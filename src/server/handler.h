#pragma once

#include "player.h"
#include "cmd.h"

extern void *initPlayersArray(void);
extern bool addPlayer(player_t *);
extern bool removePlayer(size_t);
extern bool setNicknamePlayer(size_t, char *);