#include "map.h"

/**
 * @brief Inizializza la mappa per un giocatore
 * 
 * @note La mappa viene inizializzata con tutti caratteri ```'0'```
 * 
 * @param[in] _player Giocatore a cui inizializzare la mappa
 * @return Ritorna se l'azione è stata eseguita correttamente o meno
 * 
 * @retval ```true``` Mappa inizializzata correttamente
 * @retval ```false``` La mappa non è stata inizializzata - viene impostato errno
 */
bool initPlayerMap(player_t * const _player) {
    errno = 0;

    map_t *map = (map_t *) malloc(sizeof(map_t));
    if(map == NULL) return false;
    BZERO(map, sizeof(*map));

    map->grid = (cell_t **) malloc(sizeof(cell_t *) * MAP_SIZE);
    if(map->grid == NULL) {
        free(map);
        return false;
    }
    BZERO(map->grid, sizeof(cell_t *) * MAP_SIZE);

    for(uint8_t i=0; i<MAP_SIZE; i++) {
        map->grid[i] = (cell_t *) malloc(sizeof(cell_t) * MAP_SIZE);
        if(map->grid[i] == NULL) {
            for(uint8_t j=0; j<i; j++) {
                free(map->grid[j]);
            }
            free(map->grid);
            free(map);
            return false;
        }
        memset(map->grid[i], '0', sizeof(cell_t) * MAP_SIZE);
    }

    map->ships = (ship_t *) malloc(sizeof(ship_t) * SHIPS_NUM);
    if(map->ships == NULL) {
        for(uint8_t i=0; i<MAP_SIZE; i++) {
            free(map->grid[i]);
        }
        free(map->grid);
        free(map);
        return false;
    }

    _player->map = map;
    return true;

}

/**
 * @brief Costruisce la mappa per un giocatore a partire dalle navi
 * 
 * @note La struttura del giocatore deve già contenere le navi posizionate per poter costruire la griglia della mappa
 * 
 * @param[in] _player Giocatore a cui costruire la mappa
 */
void makeMap(player_t * const _player){

    uint8_t aux;
    uint8_t x, y, dim;
    
    for(uint8_t k=0; k<SHIPS_NUM; k++){

        x = _player->map->ships[k].x;
        y = _player->map->ships[k].y;
        dim = _player->map->ships[k].dim;

        switch(_player->map->ships[k].dir){

            case 'W':
                for(aux=y; aux>y-dim; aux--){
                    _player->map->grid[aux][x]='1';
                }
                break;

            case 'A':
                for(aux=x; aux>x-dim; aux--){
                    _player->map->grid[y][aux]='1';
                }
                break;

            case 'S':
                for(aux=y; aux<y+dim; aux++){
                    _player->map->grid[aux][x]='1';
                }
                break;

            case 'D':
                for(aux=x; aux<x+dim; aux++){
                    _player->map->grid[y][aux]='1';
                }
                break;

            default: break;
        }

    }

}