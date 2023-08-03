#pragma once

#include "../shared/lib.h"
#include "../shared/cmd.h"
#include "player.h"

cmd_t wait_cmd(player_t *);
bool send_cmd(player_t *, cmd_t);