#pragma once

#include "../shared/config.h"
#include "../shared/lib.h"
#include "../shared/cmd.h"

#include "helpers.h"
#include "type.h"

void gameInitialization(void);
void printMaps(void);
void printMap(const char * encoded, bool show_all);
void makeMove(void);
uint8_t choosePlayer(const bool _insert_me);