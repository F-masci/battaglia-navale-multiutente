#pragma once

#include "../lib/lib.h"
#include "../config/config.h"
#include "../config/cmd.h"
#include "player.h"
#include "cmd.h"

extern player_t *createPlayer(int);
extern void *initPlayersArray(void);
extern bool addPlayer(player_t *);
extern bool removePlayer(size_t);
extern bool setNicknamePlayer(size_t, char *);

extern size_t n_players;                                // Numero di giocatori in lobby
extern player_t **players;                              // Array di giocatori

extern pthread_t *wthreads;                             // Waiting threads
extern pthread_t *hthreads;                             // Handler threads