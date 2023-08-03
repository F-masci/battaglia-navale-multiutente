#pragma once

#include "../lib/lib.h"
#include "../config/cmd.h"
#include "player.h"

cmd_t wait_cmd(player_t *);
bool send_cmd(player_t *, cmd_t);