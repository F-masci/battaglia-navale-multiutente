#pragma once

#define ADDRESS INADDR_ANY
#define PORT 6500
#define UDP_PORT_SRV 6501
#define UDP_PORT_CLN 6502

#define WAITING_THREADS 5
#define PENDING 30

#define DEFAULT_PLAYERS_LEN 2
#define NICKNAME_LEN 256

#define MAP_SIZE 10

// Number of ships to place in the map
#define CARRIER 1                   // Dim 5
#define BATTLESHIP 1                // Dim 4
#define SUB 3                       // Dim 3
#define DESTROYER 3                 // Dim 2 

#define SHIPS_NUM (CARRIER + BATTLESHIP + SUB + DESTROYER)