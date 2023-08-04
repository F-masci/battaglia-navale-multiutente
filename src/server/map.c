#include "map.h"

void initPlayerMap(player_t *player) {
    map_t *map = (map_t *) malloc(sizeof(map_t));
    bzero(map, sizeof(*map));
    map->grid = (cell_t **) malloc(sizeof(cell_t *)* MAP_SIZE);
    for(uint8_t i=0; i<MAP_SIZE; i++) {
        map->grid[i] = (cell_t *)malloc(sizeof(cell_t) * MAP_SIZE);
    }

    map->ships = (ship_t *) malloc(sizeof(ship_t) * SHIPS_NUM);

    player->map = map;

}

void printMap(cell_t **map) {

    PRINT("\n    ");

    for(cell_t i=0; i<MAP_SIZE; i++){
        PRINT(" %d  ", i);
    }

    PRINT("\n    ");

    for(cell_t i=0; i<MAP_SIZE; i++){
        PRINT("----");
    }

    PRINT("\n");

    for(cell_t i=0; i<MAP_SIZE; i++){
        PRINT(" %d ", i);
        for(cell_t j=0; j<MAP_SIZE; j++){
            switch(map[i][j]){
                case '0':
                    PRINT("|   ");
                    break;
                case '1':
                    PRINT("| X ");
                    break;
                default: break;
            }
        }
        PRINT("|\n");
    }

    return;
}