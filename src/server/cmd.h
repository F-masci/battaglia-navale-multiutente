#pragma once

#include "../shared/lib.h"
#include "../shared/cmd.h"
#include "player.h"

cmd_t waitCmd(player_t *);
bool sendCmd(player_t *, cmd_t);