#pragma once


// Number of ships to place in the map
#define CARRIER 1                   // Dim 5
#define BATTLESHIP 1                // Dim 4
#define SUB 3                       // Dim 3
#define DESTROYER 3                 // Dim 2 


#define MAP_SIZE 10

typedef struct _ship_t{
    int dim;
    int x;
    int y;
    char dir;
} ship_t;

