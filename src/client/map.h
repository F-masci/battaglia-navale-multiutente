#pragma once

#include "../shared/lib.h"
#include "../shared/config.h"
#include "../shared/cmd.h"

// Number of ships to place in the map
#define CARRIER 1                   // Dim 5
#define BATTLESHIP 1                // Dim 4
#define SUB 3                       // Dim 3
#define DESTROYER 3                 // Dim 2 

#include "type.h"
#include "cmd.h"

void printMap(void);
void mapInitialization(void);