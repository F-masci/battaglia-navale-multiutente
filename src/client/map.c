#include "map.h"

extern int socket_client;
extern cell_t **map;

int size=0;
ship_t *ships;
int *c; //counters for how many ships are left to place in the map

#define BUFF_LEN 1024
void mapInitialization(void) {

    int  option=0;
    int n=DESTROYER + SUB + BATTLESHIP + CARRIER; //total number of ships

    c=(int *)malloc(4*sizeof(int));
    ships=(ship_t *)malloc(n*sizeof(ship_t));

    /* --- MAP INITIALIZATION --- */
    for(int i=0; i<MAP_SIZE; i++){
        for(int j=0; j<MAP_SIZE; j++){
            map[i][j]='0';
        }
    }

    c[0]=DESTROYER;
    c[1]=SUB;
    c[2]=BATTLESHIP;
    c[3]=CARRIER;

    PRINT("\nPOSIZIONAMENTO DELLE NAVI NELLA MAPPA\n");

    while(option!=3){

        print_map();

        PRINT("\nSeleziona un comando: \n\n")
        PRINT("\t[1] POSIZIONA NAVE\n")
        PRINT("\t[2] ELIMINA NAVE\n")
        PRINT("\t[3] INVIA MAPPA\n")
        PRINT("\nScegli: ")
        retry1:
        scanf("%d", &option);
        if(option<1 || option>3) goto retry1;
        
        switch(option){
            case 1:
                place_ship();
                break;
            case 2:
                delete_ship();
                break;
            case 3:
                send_map();
                break;
            default: break;

        }
    }

    return;

}

void send_map(void) {

    int p=0;
    char *map_encoded=(char *)malloc(MAP_SIZE*MAP_SIZE*sizeof(char));
    char *buffer = (char *)malloc(sizeof(*buffer) * BUFF_LEN);
    char *ships_encoded=(char *)malloc(sizeof(ships));
    
    for(int i=0; i<MAP_SIZE; i++){
        for(int j=0; j<MAP_SIZE; j++){
            //sprintf(map_encoded+p, "%c", map[i][j]);
            map_encoded[p]=map[i][j];
            p++;
        }
    }

    p=0;
    for(int k=0; k<size; k++){
        sprintf(&ships_encoded[p], "%d", ships[k].dim);
        sprintf(&ships_encoded[++p], "%d", ships[k].x);
        sprintf(&ships_encoded[++p], "%d", ships[k].y);
        ships_encoded[++p]=ships[k].dir;
        p++;
    }

    //PRINT("\nships encoded: %s\n", ships_encoded);
    sprintf(buffer, "%hhu", CMD_SEND_MAP);
    write(socket_client, buffer, strlen(buffer));
    write(socket_client, map_encoded, strlen(map_encoded));
    write(socket_client, ships_encoded, strlen(map_encoded));
    PRINT("\nMappa inviata al server\n");
    free(map_encoded);

    return;
}

void delete_ship(void) {

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

void print_map(void) {

    clrscr();

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

void place_ship(void) {

    int i, dim=0, x, y, choice=0;
    char dir;

    if(c[0] || c[1] || c[2] || c[3]){
        PRINT("\nNavi da posizionare: \n");
        PRINT("\t[1] %d DESTROYER (DIM 2)\n", c[0]);
        PRINT("\t[2] %d SUBMARINE (DIM 3)\n", c[1]);
        PRINT("\t[3] %d BATTLESHIP (DIM 4)\n", c[2]);
        PRINT("\t[4] %d CARRIER (DIM 5)\n", c[3]);

        retry2:
            PRINT("Scegli: ");
            scanf("%d", &choice);
            if(choice<1 || choice>4) goto retry2;

            switch(choice){
                case 1:
                    if(c[0]<=0) goto retry2;
                    dim=2;
                    break;
                case 2:
                    if(c[1]<=0) goto retry2;
                    dim=3;
                    break;
                case 3: 
                    if(c[2]<=0) goto retry2;
                    dim=4;
                    break;
                case 4:
                    if(c[3]<=0) goto retry2;
                    dim=5;
                    break;
                default: break;
            }

        //coordinates go from 0 to MAP_SIZE-1
        retry_choice:
            PRINT("Scegli coordinate [x y]: ");
            scanf("%d %d", &x, &y);
            while(getchar()!='\n');

            PRINT("Scegli direzione [W/A/S/D]: ");
            scanf("%c", &dir);
            while(getchar()!='\n');


        switch(dir){
            case 'W':
                if(y-dim<0) goto retry_choice;
                for(i=0; i<dim; i++){
                    if(map[y-i][x]=='1') goto retry_choice;
                }
                for(i=0; i<dim; i++){
                    map[y-i][x]='1';
                }
                break;
            case 'A':
                if(x-dim<0) goto retry_choice;
                for(i=0; i<dim; i++){
                    if(map[y][x-i]=='1') goto retry_choice;
                }
                for(i=0; i<dim; i++){
                    map[y][x-i]='1';
                }
                break;
            case 'S':
                if(y+dim>MAP_SIZE) goto retry_choice;
                for(i=0; i<dim; i++){
                    if(map[y+i][x]=='1') goto retry_choice;
                }
                for(i=0; i<dim; i++){
                    map[y+i][x]='1';
                }
                break;
            case 'D':
                if(x+dim>MAP_SIZE) goto retry_choice;
                for(i=0; i<dim; i++){
                    if(map[y][x+i]=='1') goto retry_choice;
                }
                for(i=0; i<dim; i++){
                    map[y][x+i]='1';
                }
                break;
            default: break;
        }

        ships[size].dim=dim;
        ships[size].x=x;
        ships[size].y=y;
        ships[size].dir=dir;
        size++;
        c[choice-1]--;
    }
    else PRINT("\nNon ci sono più navi da posizionare\n");

    return;

}
#undef BUFF_LEN