#include "map.h"

extern int socket_client;
extern cell_t **map;

int size=0;
ship_t *ships;
uint8_t *c; //counters for how many ships are left to place in the map
uint8_t n;  // Total number of ships

static void _place_ship(void);
static void _delete_ship(void);
static void _send_map(void);

#define BUFF_LEN 1024
void printMap(void) {

    clrscr();

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
        for(cell_t j=0; j<MAP_SIZE; j++) {
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
    PRINT("    ");
    for(cell_t i=0; i<MAP_SIZE; i++){
        PRINT("----");
    }

}

void mapInitialization(void) {

    n = DESTROYER + SUB + BATTLESHIP + CARRIER;

    c = (uint8_t *) malloc( 4 * sizeof(*c));
    ships = (ship_t *) malloc ( n * sizeof(*ships));

    /* -- MAP INITIALIZATION -- */

    for(int i=0; i<MAP_SIZE; i++){
        for(int j=0; j<MAP_SIZE; j++){
            map[i][j] = '0';
        }
    }

    /* -- SHIPS TO PLACE -- */

    c[0] = DESTROYER;
    c[1] = SUB;
    c[2] = BATTLESHIP;
    c[3] = CARRIER;

    PRINT("\nPOSIZIONAMENTO DELLE NAVI NELLA MAPPA\n");

    uint8_t cmd;

    /* -- LOOP -- */

map_init_loop:

    printMap();

    PRINT("\nSeleziona un comando: \n\n")
    PRINT("\t[1] POSIZIONA NAVE\n")
    PRINT("\t[2] ELIMINA NAVE\n")
    PRINT("\t[3] INVIA MAPPA\n")
    PRINT("\nScegli: ")
    if(scanf("%hhu", &cmd) <= 0) {
        while((getchar()) != '\n');
        goto map_init_loop;
    }
    
    switch(cmd) {
        case 1:
            _place_ship();
            break;
        case 2:
            _delete_ship();
            break;
        case 3:
            if(size == SHIPS_NUM) {
                _send_map();
                return;
            } else {
                PRINT("Inserisci prima tutte le navi\n")
                break;
            }
        default: break;

    }

    goto map_init_loop;

    return;

}

static void _place_ship(void) {

    char dir;
    uint8_t cmd, dim;
    cell_t x, y;

    if(c[0] || c[1] || c[2] || c[3]) {
        PRINT("\nNavi da posizionare: \n");
        PRINT("\t[1] %d DESTROYER (DIM 2)\n", c[0]);
        PRINT("\t[2] %d SUBMARINE (DIM 3)\n", c[1]);
        PRINT("\t[3] %d BATTLESHIP (DIM 4)\n", c[2]);
        PRINT("\t[4] %d CARRIER (DIM 5)\n", c[3]);

place_ship_loop:
        PRINT("Scegli: ");
        if(scanf("%hhu", &cmd) <= 0) {
            while((getchar()) != '\n');
            goto place_ship_loop;
        }

        switch(cmd){
            case 1:
                if(c[0]<=0) goto place_ship_loop;
                dim=2;
                break;
            case 2:
                if(c[1]<=0) goto place_ship_loop;
                dim=3;
                break;
            case 3: 
                if(c[2]<=0) goto place_ship_loop;
                dim=4;
                break;
            case 4:
                if(c[3]<=0) goto place_ship_loop;
                dim=5;
                break;
            default: goto place_ship_loop;
        }

        //coordinates go from 0 to MAP_SIZE-1
retry_choice:
        PRINT("Scegli coordinate [x y]: ");
        if(scanf("%hhu %hhu", &x, &y) <= 0) {
            while((getchar()) != '\n');
            goto retry_choice;
        }
        while((getchar()) != '\n');

        PRINT("Scegli direzione [W/A/S/D]: ");
        if(scanf("%c", &dir) <= 0) {
            while((getchar()) != '\n');
            goto retry_choice;
        }
        while((getchar()) != '\n');

        switch(toupper(dir)){
            case 'W':
                if(y-dim<0) goto retry_choice;
                for(uint8_t i=0; i<dim; i++){
                    if(map[y-i][x] == (cell_t) '1') goto retry_choice;
                }
                for(uint8_t i=0; i<dim; i++){
                    map[y-i][x] = (cell_t) '1';
                }
                break;
            case 'A':
                if(x-dim<0) goto retry_choice;
                for(uint8_t i=0; i<dim; i++){
                    if(map[y][x-i] == (cell_t) '1') goto retry_choice;
                }
                for(uint8_t i=0; i<dim; i++){
                    map[y][x-i] = (cell_t) '1';
                }
                break;
            case 'S':
                if(y+dim>MAP_SIZE) goto retry_choice;
                for(uint8_t i=0; i<dim; i++){
                    if(map[y+i][x] == (cell_t) '1') goto retry_choice;
                }
                for(uint8_t i=0; i<dim; i++){
                    map[y+i][x] = (cell_t) '1';
                }
                break;
            case 'D':
                if(x+dim>MAP_SIZE) goto retry_choice;
                for(uint8_t i=0; i<dim; i++){
                    if(map[y][x+i] == (cell_t) '1') goto retry_choice;
                }
                for(uint8_t i=0; i<dim; i++){
                    map[y][x+i] = (cell_t) '1';
                }
                break;
            default: break;
        }

        ships[size].dim = dim;
        ships[size].x = x;
        ships[size].y = y;
        ships[size].dir = toupper(dir);
        size++;
        c[cmd-1]--;
    }
    else PRINT("\nNon ci sono più navi da posizionare\n");

    return;

}

static void _delete_ship(void) {

    int choice, i, x, y, dim;

    if(size<1){
        PRINT("\nNessuna nave da eliminare\n");
        return;
    }

    PRINT("\nNavi disponibili\n");
    for(i=0; i<size; i++){
        PRINT("\t[%d] ", i);
        switch(ships[i].dim){
            case 2:
                PRINT("DESTROYER (%d,%d)\n", ships[i].x, ships[i].y);
                break;
            case 3:
                PRINT("SUBMARINE (%d,%d)\n", ships[i].x, ships[i].y);
                break;
            case 4:
                PRINT("BATTLESHIP\n");
                break;
            case 5:
                PRINT("CARRIER\n");
                break;
            default: break;
        }
    }

    try_again:
        PRINT("Scegli: ");
        scanf("%d", &choice);
        if(choice<0 || choice>=size) goto try_again;

    x=ships[choice].x;
    y=ships[choice].y;
    dim=ships[choice].dim;
    c[dim-2]++;

    switch(ships[choice].dir){
        case 'W':
            for(i=y; i>y-dim; i--){
                map[i][x]='0';
            }
            break;
        case 'A':
            for(i=x; i>x-dim; i--){
                map[y][i]='0';
            }
            break;
        case 'S':
            for(i=y; i<y+dim; i++){
                map[i][x]='0';
            }
            break;
        case 'D':
            for(i=x; i<x+dim; i++){
                map[y][i]='0';
            }
            break;
        default: break;
    }

    for(i=choice; i<size-1; i++){
        ships[i]=ships[i+1];
    }
    size--;

    return;

}

static void _send_map(void) {

    sendCmd(CMD_SEND_MAP);

    char *ships_encoded = (char *) malloc( n * sizeof(*ships) + 1);        // Calcolo lo spazio di cui ho bisogno per rappresentare le navi + il terminatore
    bzero(ships_encoded, sizeof(*ships_encoded));

    char *cur = ships_encoded;

    for(int k=0; k<size; k++){
        *cur++ = '0' + ships[k].dim;
        *cur++ = '0' + ships[k].x;
        *cur++ = '0' + ships[k].y;
        *cur++ = ships[k].dir;
    }

    *cur = 0;   // Inserisco il terminatore

    write(socket_client, ships_encoded, strlen(ships_encoded));
    PRINT("\nMappa inviata al server\n");
    free(ships_encoded);

    return;
}

#undef BUFF_LEN