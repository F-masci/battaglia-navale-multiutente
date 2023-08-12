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

#include "client.h"
#include "game.h"

struct sockaddr_in addr_server;
int socket_client;
cell_t **map;

#define BUFF_LEN 1024
int main(void) {

    int i;

    /* -- CONFIG SERVER ADDRESS -- */

    bzero((char*) &addr_server, sizeof(addr_server));
    addr_server.sin_family = AF_INET;
    addr_server.sin_port = htons(PORT);     // 6500
    addr_server.sin_addr.s_addr = ADDRESS;  // 0.0.0.0

    clientConnection();                     // CLIENT CONNECTION

    map = (cell_t **) malloc(MAP_SIZE * sizeof(*map));
    for(i=0; i<MAP_SIZE; i++) map[i] = (cell_t *) malloc(MAP_SIZE * sizeof(*(map[i])));

    mapInitialization();                   //MAP INITIALIZATION

    cmd_t cmd;
    char *buffer = (char *)malloc(sizeof(char) * BUFF_LEN);

wait_turn:

    cmd = waitCmd();
    if(cmd != CMD_TURN) goto wait_turn;

    PRINT("È il tuo turno\n")

    PRINT("\nSeleziona un comando:\n\n")
    PRINT("\t[1] Visualizza mappe giocatori\n")
    PRINT("\t[2] Invia comando\n\n")

main_loop:

    PRINT("Comando: ")
    if(scanf("%hhu", &cmd) <= 0) {
        while((getchar()) != '\n');
        goto main_loop;
    }

    bzero(buffer, BUFF_LEN);
    switch(cmd) {
        case 1: 
            sendCmd(CMD_GET_MAP);
            print_maps();
            goto main_loop;

        case 2: 
            sendCmd(CMD_MOVE);
            goto wait_turn;

        default: goto main_loop;
    }

    return 0;

}
#undef BUFF_LEN