#pragma once

#include "../shared/lib.h"
#include "../shared/cmd.h"
#include "player.h"

cmd_t waitCmd(const player_t * const _player);
bool sendCmd(const player_t * const _player, const cmd_t _cmd);