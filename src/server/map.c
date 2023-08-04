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

void print_map(cell_t **map) {

    int i, j;

    PRINT("\n    ");
    for(i=0; i<MAP_SIZE; i++){
        PRINT(" %d  ", i);
    }
    PRINT("\n    ");
    for(i=0; i<MAP_SIZE; i++){
        PRINT("----");
    }
    PRINT("\n");

    for(i=0; i<MAP_SIZE; i++){
        PRINT(" %d ", i);
        for(j=0; j<MAP_SIZE; j++){
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