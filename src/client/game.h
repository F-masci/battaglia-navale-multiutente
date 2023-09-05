#pragma once

#include "../shared/config.h"
#include "../shared/lib.h"

#include "helpers.h"

void print_maps(void);
uint8_t choose_player(int option);
void gameInitialization(void);
void print_map(void);
void make_map(void);
void map_print(int option, char *encoded);