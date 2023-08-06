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
                case '2':
                    PRINT("| K ");
                    break;
                default: break;
            }
        }
        PRINT("|\n");
    }

    return;
}

void makeMap(player_t *player){

    /* --- MAP INITIALIZATION --- */

    int i, j;
    uint8_t x, y, dim;
    
    for(i=0; i<MAP_SIZE; i++){
        for(j=0; j<MAP_SIZE; j++){
            player->map->grid[i][j]='0';
        }
    }

    /* --- MAP RECONSTRUCTION FROM SHIPS --- */

    for(int k=0; k<SHIPS_NUM; k++){
        x=player->map->ships[k].x;
        y=player->map->ships[k].y;
        dim=player->map->ships[k].dim;

        switch(player->map->ships[k].dir){
            case 'W':
                for(i=y; i>y-dim; i--){
                    player->map->grid[i][x]='1';
                }
                break;
            case 'A':
                for(i=x; i>x-dim; i--){
                    player->map->grid[y][i]='1';
                }
                break;
            case 'S':
                for(i=y; i<y+dim; i++){
                    player->map->grid[i][x]='1';
                }
                break;
            case 'D':
                for(i=x; i<x+dim; i++){
                    player->map->grid[y][i]='1';
                }
                break;
            default: break;
        }
    }

    return;

}