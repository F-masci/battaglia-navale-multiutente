/**
 * @authors Cacace Elisa, Masci Francesco
 * 
 * Battaglia navale multiutente
 * 
 * Realizzazione di una versione elettronica del famoso gioco "battaglia
 * navale" con un numero di giocatori arbitrario. In questa versione piu'
 * processi client (residenti in generale su macchine diverse) sono l'interfaccia tra i
 * giocatori e il server (residente in generale su una macchina separata dai
 * client). Un client, una volta abilitato dal server, accetta come input una
 * mossa, la trasmette al server, e riceve la risposta dal server. In questa
 * versione della battaglia navale una mossa consiste oltre alle due coordinate
 * anche nell'identificativo del giocatore contro cui si vuole far fuoco.
 * Il server a sua volta quando riceve una mossa, comunica ai client se
 * qualcuno e' stato colpito se uno dei giocatori e' il vincitore (o se e' stato
 * eliminato), altrimenti abilita il prossimo client a spedire una mossa.
 * La generazione della posizione delle navi per ogni client e' lasciata alla
 * discrezione dello studente. 
 *
*/

#include "../lib/lib.h"
#include "../config/config.h"
#include "../lib/client.h"
#include "../config/map.h"

struct sockaddr_in addr_server;
char **map;


int main(void) {

    int i;
    int c[4]; //counters for how many ships are left to place in the map
    int choice=-1;
    int dim=0, x, y;
    char dir;

    /* -- CONFIG SERVER ADDRESS -- */

    bzero((char*) &addr_server, sizeof(addr_server));
    addr_server.sin_family = AF_INET;
    addr_server.sin_port = htons(PORT);     // 6500
    addr_server.sin_addr.s_addr = ADDRESS;  // 0.0.0.0

    clientConnection();   
                      // CLIENT CONNECTION

    map=(char **)malloc(MAP_SIZE*sizeof(char *));
    for(i=0; i<MAP_SIZE; i++) map[i]=(char *)malloc(MAP_SIZE*sizeof(char));

    /*-- MAP INITIALIZATION --*/

    for(i=0; i<MAP_SIZE; i++){
        for(int j=0; j<MAP_SIZE; j++){
            map[i][j]=0;
        }
    }

    c[0]=DESTROYER;
    c[1]=SUB;
    c[2]=BATTLESHIP;
    c[3]=CARRIER;

    PRINT("\nPOSIZIONARE LE NAVI NELLA MAPPA\n");
    
    while(c[0] || c[1] || c[2] || c[3]){

        print_map(map);

        PRINT("\nNavi da posizionare: \n");
        PRINT("1. %d DESTROYER (DIM 2)\n", c[0]);
        PRINT("2. %d SUBMARINE (DIM 3)\n", c[1]);
        PRINT("3. %d BATTLESHIP (DIM 4)\n", c[2]);
        PRINT("4. %d CARRIER (DIM 5)\n", c[3]);

        //aggiungere controllo input 

        PRINT("Scegli: ");
        scanf("%d", &choice);

        switch(choice){
            case 1:
                dim=2;
                break;
            case 2:
                dim=3;
                break;
            case 3: 
                dim=4;
                break;
            case 4:
                dim=5;
                break;
            default: break;
        }

        //coordinates go from 0 to MAP_SIZE-1
    retry_choice:
        PRINT("Scegli coordinate [x, y]: ");
        scanf("%d %d", &x, &y);
        while(getchar()!='\n');

        PRINT("Scegli direzione [W/A/S/D]: ");
        scanf("%c", &dir);
        while(getchar()!='\n');

        if(place_ship(map, dim, dir, x, y)==-1) goto retry_choice;
        
        c[choice-1]--;
    }

}

