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

#include "server.h"

int socket_server;
struct sockaddr_in addr_server;

uint8_t n_players = 0;                          // Numero di giocatori in lobby
player_t **players = NULL;                      // Array di puntatori ai metadati dei giocatori
pthread_t *w_threads;                           // Waiting threads
int semid;                                      // Sempahore to sync map receive

int main() {

    /* -- INIT GLOBAL VARS -- */

    initPlayersArray();
    w_threads = (pthread_t *) malloc(sizeof(*w_threads) * WAITING_THREADS);
    semid = semget(IPC_PRIVATE, 1, IPC_CREAT | 0600);

    /* -- CONFIG SERVER ADDRESS -- */

    bzero((char*) &addr_server, sizeof(addr_server));
    addr_server.sin_family = AF_INET;
    addr_server.sin_port = htons(PORT);     // 6500
    addr_server.sin_addr.s_addr = ADDRESS;  // 0.0.0.0

    waitConnections();                      // Create lobby

    /* -- WAITING FOR ALL MAPS -- */

    semctl(semid, (int) 0, SETVAL, 0);

    struct sembuf so;
    bzero(&so, sizeof(so));
    so.sem_num = 0;
    so.sem_op = (short) -n_players;
    so.sem_flg = 0;
    semop(semid, &so, 1);

    PRINT("[SERVER] All map configured\n")

    /* -- GAME -- */

    gameInitialization();               //INITIALIZATION

    PRINT("[SERVER] Game initialization done\n")

    size_t index = 0;
    cmd_t cmd;

    PRINT("[SERVER] Starting game\n")

main_loop:

    // Start player turn
    sendCmd(players[index], CMD_TURN);

main_cmd_loop:
    cmd = waitCmd(players[index]);
    PRINT("[%s]: request command %hu\n", players[index]->nickname, cmd)
    switch(cmd) {

        case CMD_GET_MAPS: 
            send_maps(players[index]);
            goto main_cmd_loop;

        case CMD_GET_MAP:
            send_map(players[index]);
            goto main_cmd_loop;

        case CMD_MOVE:
            get_move(players[index]);
            break;

        case CMD_ERROR:
            return EXIT_FAILURE;

        default: goto main_cmd_loop;
    }

    if(n_players == 1){
        PRINT("[SERVER] player %s has won\n", players[0]->nickname)
        return EXIT_SUCCESS;
    }

    index = (index+1)%n_players;
    goto main_loop;

    return EXIT_SUCCESS;

}