#pragma once

#include "../shared/lib.h"
#include "player.h"

bool waitNum(const player_t * const _player, uint32_t * const _ptr);
bool writeNum(const player_t * const _player, uint32_t _num);
bool waitString(const player_t * const _player, char ** const _buff);
bool writeString(const player_t * const _player, const char * _buff);