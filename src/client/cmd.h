#pragma once

#include "../shared/lib.h"
#include "../shared/cmd.h"

cmd_t waitCmd(void);
bool sendCmd(const cmd_t _cmd);