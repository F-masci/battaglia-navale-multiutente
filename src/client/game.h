#pragma once

#include "../shared/config.h"
#include "../shared/lib.h"

#include "helpers.h"

void print_maps(void);
uint8_t choose_player(int option);
void gameInitialization(uint8_t time);
void print_map(void);
void make_move(void);
void map_print(int option, char *encoded);