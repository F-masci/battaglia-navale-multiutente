#pragma once

#include "../shared/lib.h"
#include "player.h"

bool waitString(player_t *, char **);
bool writeString(player_t *, char *);
bool waitNum(player_t *, uint32_t *);
bool writeNum(player_t *, uint32_t);