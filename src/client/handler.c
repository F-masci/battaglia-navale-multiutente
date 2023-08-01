#include "../lib/lib.h"
#include "../config/map.h"

void print_map(char **map){

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
                case 0:
                    PRINT("|   ");
                    break;
                case -1:
                    PRINT("| X ");
                    break;
                default: break;
            }
        }
        PRINT("|\n");
    }

    return;
}

int place_ship(char **map, int dim, char dir, int x, int y){

    int i;

    switch(dir){
        case 'W':
            if(y-dim<0) return -1;
            for(i=0; i<dim; i++){
                if(map[y-i][x]==-1) return -1;
            }
            for(i=0; i<dim; i++){
                map[y-i][x]=-1;
            }
            break;
        case 'A':
            if(x-dim<0) return -1;
            for(i=0; i<dim; i++){
                if(map[y][x-i]==-1) return -1;
            }
            for(i=0; i<dim; i++){
                map[y][x-i]=-1;
            }
            break;
        case 'S':
            if(y+dim>MAP_SIZE) return -1;
            for(i=0; i<dim; i++){
                if(map[y+i][x]==-1) return -1;
            }
            for(i=0; i<dim; i++){
                map[y+i][x]=-1;
            }
            break;
        case 'D':
            if(x+dim>MAP_SIZE) return -1;
            for(i=0; i<dim; i++){
                if(map[y][x+i]==-1) return -1;
            }
            for(i=0; i<dim; i++){
                map[y][x+i]=-1;
            }
            break;
        default: break;
    }

    return 0;

}