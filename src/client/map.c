#include "map.h"

extern int socket_client;

uint8_t size=0;
ship_t *ships;
uint8_t *c;         //Contatori per quante navi di ogni tipo sono ancora da posizionare nella mappa
uint8_t n;          //Numero totale di navi posizionate

static cell_t **map = NULL;

static void _place_ship(void);
static void _delete_ship(void);
static bool _sendMap(void);

/**
 * @brief Funzione per l'inizializzazione della mappa + gestione dell'inserimento delle navi da parte del giocatore
 *
 * @note La mappa viene inizialmente riempita di ```0```
*/

void mapInitialization(void) {

    n = DESTROYER + SUB + BATTLESHIP + CARRIER;

    c = (uint8_t *) malloc( 4 * sizeof(*c));
    ships = (ship_t *) malloc ( n * sizeof(*ships));

    /* -- INIZIALIZZAZIONE MAPPA -- */

    map = (cell_t **) malloc(MAP_SIZE * sizeof(*map));
    if(map == NULL) EXIT_ERRNO
    for(uint8_t i=0; i<MAP_SIZE; i++) {
        map[i] = (cell_t *) malloc(MAP_SIZE * sizeof(*(map[i])));
        if(map[i] == NULL) EXIT_ERRNO
        memset(map[i], '0', MAP_SIZE * sizeof(cell_t));
    }

    /* -- NAVI DA POSIZIONARE -- */

    c[0] = DESTROYER;
    c[1] = SUB;
    c[2] = BATTLESHIP;
    c[3] = CARRIER;

    /* -- LOOP -- */

    uint8_t cmd;

map_init_loop:

    /* -- STAMPA MAPPA -- */

    clrscr();

    PRINT("\n     POSIZIONAMENTO DELLE NAVI NELLA MAPPA\n");

    PRINT("\n    ");
    for(uint8_t i=0; i<MAP_SIZE; i++){
        PRINT(" %c  ", 'A' + i); 
    }
    PRINT("\n    ");
    for(uint8_t i=0; i<MAP_SIZE; i++){
        PRINT("----");
    }
    PRINT("\n");

    for(uint8_t i=0; i<MAP_SIZE; i++){
        PRINT(" %d ", i); 
        for(uint8_t j=0; j<MAP_SIZE; j++) {
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
    for(uint8_t i=0; i<MAP_SIZE; i++){
        PRINT("----");
    }

    /* -- SELEZIONE CMD -- */

    PRINT("\nSeleziona un comando: \n\n")
    PRINT("\t[1] POSIZIONA NAVE\n")
    PRINT("\t[2] ELIMINA NAVE\n")
    PRINT("\t[3] INVIA MAPPA\n")
    PRINT("\nScegli: ")

    if(scanf("%hhu", &cmd) <= 0) {
        EXIT_ERRNO
        while((getchar()) != '\n');
        goto map_init_loop;
    }
    while((getchar()) != '\n');
    
    switch(cmd) {
        case 1:
            _place_ship();
            break;
        case 2:
            _delete_ship();
            break;
        case 3:
            if(_sendMap()) return;
            else break;

        default: break;

    }

    goto map_init_loop;

    return;

}

/**
 * @brief Funzione per posizionare una nave nella propria mappa
 *
 * @note L'inserimento è possibile finché non sono state posizionate tutte le navi disponibili
*/

static void _place_ship(void) {

    char dir;
    uint8_t cmd, dim;
    uint8_t x, y;

    if(c[0] || c[1] || c[2] || c[3]) {
        PRINT("\nNavi da posizionare: \n");
        PRINT("\t[1] %d DESTROYER (DIM 2)\n", c[0]);
        PRINT("\t[2] %d SUBMARINE (DIM 3)\n", c[1]);
        PRINT("\t[3] %d BATTLESHIP (DIM 4)\n", c[2]);
        PRINT("\t[4] %d CARRIER (DIM 5)\n", c[3]);

place_ship_loop:
        PRINT("Scegli: ");
        if(scanf("%hhu", &cmd) <= 0) {
            EXIT_ERRNO
            while((getchar()) != '\n');
            goto place_ship_loop;
        }
        while((getchar()) != '\n');

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

retry_choice:
        PRINT("Scegli cella [es. A 0]: ");
        if(scanf("%c %hhu", &x, &y) <= 0) {
            EXIT_ERRNO
            while((getchar()) != '\n');
            goto retry_choice;
        }
        while((getchar()) != '\n');

        DEBUG("%c %hhu\n", x, y);

        //Converte lettera in coordinata numerica nella mappa

        if( (x < 'A' || x > 'J') && (x < 'a' || x > 'j')) goto retry_choice;

        x = toupper(x) - 'A';
        
        if(x > 9 || y > 9) goto retry_choice;

        DEBUG("%hhu %hhu\n", x, y);

        PRINT("Scegli direzione [W/A/S/D]: ");
        if(scanf("%c", &dir) <= 0) {
            EXIT_ERRNO
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
    else{
        PRINT("\nNon ci sono più navi da posizionare\n");
        sleep(2);
    }

    return;

}

/**
 * @brief Funzione per eliminare una delle navi già posizionate
 *
 * @note Viene mostrata una lista delle navi che sono state posizionate fino ad ora tra le quali il giocatore può scegliere
*/

static void _delete_ship(void) {

    uint8_t choice, x, y, dim;

    if(size < 1){
        PRINT("\nNessuna nave da eliminare\n");
        return;
    }

    PRINT("\nNavi disponibili\n\n");
    for(uint8_t i=0; i<size; i++){
        PRINT("\t[%d] ", i);
        switch(ships[i].dim){
            case 2:
                PRINT("DESTROYER (%c%hhu)\n", ships[i].x + 'A', ships[i].y);
                break;
            case 3:
                PRINT("SUBMARINE (%c%hhu)\n", ships[i].x + 'A', ships[i].y);
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
    PRINT("\nScegli: ");
    if(scanf("%hhu", &choice) <= 0) {
        EXIT_ERRNO
        while(getchar() != '\n');
        goto try_again;
    }
    while(getchar() != '\n');
    if(choice >= size) goto try_again;

    x = ships[choice].x;
    y = ships[choice].y;
    dim = ships[choice].dim;
    c[dim-2]++;

    switch(ships[choice].dir){
        case 'W':
            for(uint8_t i=y; i>y-dim; i--){
                map[i][x]='0';
            }
            break;
        case 'A':
            for(uint8_t i=x; i>x-dim; i--){
                map[y][i]='0';
            }
            break;
        case 'S':
            for(uint8_t i=y; i<y+dim; i++){
                map[i][x]='0';
            }
            break;
        case 'D':
            for(uint8_t i=x; i<x+dim; i++){
                map[y][i]='0';
            }
            break;
        default: break;
    }

    for(uint8_t i=choice; i<size-1; i++){
        ships[i]=ships[i+1];
    }
    size--;

}

/**
 * @brief Funzione per inviare la mappa definitiva al server
 *
 * @note L'invio è permesso solo se non ci sono più navi da posizionare
 * @note Il client invia al server una stringa in cui sono codificati gli elementi del vettore ships contenente tutte le navi posizionate
 * @return Ritorna se la mappa è stata inviata o meno
 *
 * @retval ```true``` --> mappa inviata al server
 * @retval ```false``` --> mappa non inviata
*/

static bool _sendMap(void) {

    if(size != SHIPS_NUM){
        PRINT("\nCI SONO ANCORA NAVI DA POSIZIONARE\n")
        sleep(1);
        return false;
    }

    if(!sendCmd(CMD_SEND_MAP)) EXIT_ERRNO

    char *encoded = (char *) malloc( n * sizeof(*ships) + 1);        // Calcolo lo spazio di cui ho bisogno per rappresentare le navi + il terminatore
    if(encoded == NULL) EXIT_ERRNO
    BZERO(encoded, sizeof(n * sizeof(*ships) + 1));

    char *cur = encoded;

    for(int k=0; k<size; k++){
        *cur++ = '0' + ships[k].dim;
        *cur++ = '0' + ships[k].x;
        *cur++ = '0' + ships[k].y;
        *cur++ = ships[k].dir;
    }

    *cur = 0;   // Inserisco il terminatore

    if(!writeString(encoded)) EXIT_ERRNO
    PRINT("\nMappa inviata al server\nIn attesa degli altri giocatori...\n");
    free(encoded);

    return true;

}