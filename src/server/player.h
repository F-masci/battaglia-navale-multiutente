#pragma once

#include "type.h"

#include "../shared/lib.h"
#include "../shared/config.h"
#include "map.h"

player_t *createPlayer(const int _fd_socket);
player_t **initPlayersArray(void);
bool addPlayer(player_t * const _player);
bool removePlayer(const size_t _index);
bool setNicknamePlayer(const size_t _index, const char * const _nickname);