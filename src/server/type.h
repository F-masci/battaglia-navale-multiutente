#pragma once

#include "../shared/lib.h"
#include "../shared/config.h"

typedef unsigned char cell_t;

/**
 * @brief Struttura per gestire una nave sulla mappa
 * 
 */
typedef struct _ship_t{
    uint8_t dim;        // Dimensione della nave
    uint8_t x;          // Cordinata X della casella iniziale
    uint8_t y;          // Cordinata Y della casella iniziale
    char dir;           // Direzione in cui è posizionata la nave
    bool sunk;          // Indica se la nave è stata affondata o meno
} ship_t;

/**
 * @brief Struttura per gestire la mappa di un giocatore
 * 
 */
typedef struct _map_t {
    cell_t **grid;      // Griglia completa
    ship_t *ships;      // Lista delle navi
} map_t;

/**
 * @brief Metadati di un giocatore
 * 
 */
typedef struct _player_t {
    size_t index;                       // Indice del giocatore nell'array dei giocatori
    int socket;                         // Socket aperto verso quel
    char nickname[NICKNAME_LEN];        // Nickname del giocatore
    bool ready;                         // Indica se il giocatore è pronto ad iniziare la partita
    map_t *map;                         // Puntatore alla mappa del giocatore
} player_t;