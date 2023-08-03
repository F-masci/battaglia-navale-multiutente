#include "map.h"

void initPlayerMap(player_t *player) {
    map_t *map = (map_t *) malloc(sizeof(*map));
    bzero(map, sizeof(*map));
    map->grid = (cell_t **) malloc(sizeof(*(map->grid)) * MAP_SIZE);
    for(uint8_t i=0; i<MAP_SIZE; i++) {
        map->grid[i] = (cell_t *) malloc(sizeof(*(map->grid[i])) * MAP_SIZE);
    }
    map->ships = (ship_t *) malloc(sizeof(*(map->ships)) * SHIPS_NUM);

    player->map = map;

}