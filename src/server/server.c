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
#include "../lib/server.h"
#include "player.h"

int socket_server;
struct sockaddr_in addr_server;

size_t n_players = 0;                           // Numero di giocatori in lobby
player_t **players;                             // Array di puntatori ai metadati dei giocatori

void sig_handler(int sig) {
    PRINT("Thread exit: %d.\n", gettid())
    pthread_exit(NULL);
}

int main() {

    /* -- INIT GLOBAL VARS -- */

    initPlayersArray();

    /* -- CONFIG SERVER ADDRESS -- */

    bzero((char*) &addr_server, sizeof(addr_server));
    addr_server.sin_family = AF_INET;
    addr_server.sin_port = htons(PORT);     // 6500
    addr_server.sin_addr.s_addr = ADDRESS;  // 0.0.0.0

    signal(SIGINT, sig_handler);

    waitConnections();                      // Create lobby

    return 0;
}