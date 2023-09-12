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

int socket_server;                              // Socket per il collegamento dei client al server
struct sockaddr_in addr_server;                 // Indirizzo del socket del server

uint8_t n_players = 0;                          // Numero di giocatori in lobby
player_t **players = NULL;                      // Array di puntatori ai metadati dei giocatori
pthread_t *w_threads;                           // Waiting threads
int semid = -1;                                 // Semaforo per sincronizzare la ricezione delle mappe

static void exit_function(int code) {
    if(semid != -1) {
        DEBUG("[DEBUG]: closing sem %d\n", semid);
        if(semctl(semid, 0, IPC_RMID) == -1) EXIT_ERRNO
    }
    
    for(uint8_t i=0; i<n_players; i++) {
        if(!sendCmd(players[i], CMD_CLOSE_CONNECTION)) {
            if(errno != EPIPE) {
                CHECK_ERRNO("Error");
            } else {
                errno = 0;
            }
        }
    }
    exit(code);
}

static void _sigint_main_handler(int sig, siginfo_t *dummy, void *dummy2) {
    PRINT("[SERVER]: server exit for SIGINT\n")
    exit_function(EXIT_SUCCESS);
}

static void _sigpipe_main_handler(int sig, siginfo_t *dummy, void *dummy2) {
    PRINT("[SERVER]: server exit for SIGPIPE\n")
    exit_function(EXIT_FAILURE);
}

int main() {

    /* -- SIGINT HANDLER -- */

    sigset_t set;
    sigfillset(&set);
    sigdelset(&set, SIGINT);
    if(sigprocmask(SIG_SETMASK, &set, NULL) == -1) exit(EXIT_FAILURE);
    
    struct sigaction sa;
    BZERO(&sa, sizeof(sa));
    sa.sa_sigaction = _sigint_main_handler;
    sa.sa_mask = set;
    sa.sa_flags = 0;
    sa.sa_restorer = NULL;
    if(sigaction(SIGINT, &sa, NULL) == -1) exit(EXIT_FAILURE);

    /* -- SIGPIPE HANDLER -- */

    sigemptyset(&set);
    sigaddset(&set, SIGPIPE);
    if(sigprocmask(SIG_UNBLOCK, &set, NULL) == -1) exit(EXIT_FAILURE);

    sigfillset(&set);

    BZERO(&sa, sizeof(sa));
    sa.sa_sigaction = _sigpipe_main_handler;
    sa.sa_mask = set;
    sa.sa_flags = 0;
    sa.sa_restorer = NULL;
    if(sigaction(SIGPIPE, &sa, NULL) == -1) exit(EXIT_FAILURE);

    /* -- INIT VARIABILI GLOBALI -- */

    if(initPlayersArray() == NULL) EXIT_ERRNO
    w_threads = (pthread_t *) malloc(sizeof(*w_threads) * WAITING_THREADS);
    if(w_threads == NULL) EXIT_ERRNO
    if( (semid = semget(IPC_PRIVATE, 1, IPC_CREAT | 0600)) == -1) EXIT_ERRNO

    /* -- CONFIG SERVER ADDRESS -- */

    BZERO((char*) &addr_server, sizeof(addr_server));
    addr_server.sin_family = AF_INET;
    addr_server.sin_port = htons(PORT);     // 6500
    addr_server.sin_addr.s_addr = ADDRESS;  // 0.0.0.0

    waitConnections();                      // Create lobby

    if(n_players <= 0) {
        PRINT("[SERVER]: nessun giocatore in lobby\n")
        goto main_exit;
    }

    /* -- ATTESA DI TUTTE LE MAPPE -- */

    if(semctl(semid, (int) 0, SETVAL, 0) == -1) EXIT_ERRNO;

    struct sembuf so;
    BZERO(&so, sizeof(so));
    so.sem_num = 0;
    so.sem_op = (short) -n_players;
    so.sem_flg = 0;
    while(semop(semid, &so, 1) == -1) {
        EXIT_ERRNO
        errno = 0;
    };

    PRINT("[SERVER] All map configured\n")

    if(semctl(semid, 0, IPC_RMID) == -1) EXIT_ERRNO

    DEBUG("[DEBUG] Semaphore %d closed\n", semid)
    semid = -1;

    gameInitialization();               //INIZIALIZZAZIONE DEL GIOCO

    PRINT("[SERVER] Game initialization done\n")

    size_t index = 0;                   // Indice del giocatore di turno
    cmd_t cmd;                          // Comando ricevuto

    PRINT("[SERVER] Starting game\n")

    int16_t index_elim;                 // Indice del giocatore eliminato

main_loop:

    // Inizia turno giocatore
    PRINT("[SERVER]: turn of %s\n", players[index]->nickname)
    if(!sendCmd(players[index], CMD_TURN)) {
        CHECK_ERRNO("Error")
        goto main_exit;
    }

main_cmd_loop:
    cmd = waitCmd(players[index]);
    if(cmd == CMD_ERROR) EXIT_ERRNO
    PRINT("[%s]: request command %hu\n", players[index]->nickname, cmd)
    switch(cmd) {

        case CMD_GET_MAPS: 
            if(!sendMaps(players[index])) EXIT_ERRNO
            goto main_cmd_loop;

        case CMD_GET_MAP:
            if(!sendMap(players[index])) EXIT_ERRNO
            goto main_cmd_loop;

        case CMD_MOVE:
            index_elim = getMove(players[index]);
            if(index_elim <= -1) EXIT_ERRNO
            if(index == (uint8_t) index_elim) {                 // Nessuna eliminazione
                for(uint8_t j=0; j<n_players; j++) if(!writeNum(players[j], n_players + 2)) EXIT_ERRNO
            } else {                                            // Eliminazione di index_elim
                for(uint8_t j=0; j<n_players; j++){
                    if(j != (uint8_t) index_elim) {
                        if(!writeNum(players[j], index_elim)) EXIT_ERRNO
                    }
                }
                if(!writeNum(players[index_elim], n_players + 1)) EXIT_ERRNO

                // Sistemo l'indice per il prossimo giocatore
                // Se il prossimo giocatore è il primo togliendo un giocatore egli salterà il turno
                // Dobbiamo impedire questa cosa
                if(index+1 == n_players) index--;

                if(!removePlayer(index_elim)) EXIT_ERRNO

            }
            break;

        case CMD_CLOSE_CONNECTION:
            goto main_exit;

        default: goto main_cmd_loop;
    }

    if(n_players == 1){
        PRINT("[SERVER] player %s has won\n", players[0]->nickname)
        return EXIT_SUCCESS;
    }

    index = (index+1)%n_players;
    goto main_loop;

main_exit:
    PRINT("[SERVER]: server exit\n")
    exit_function(EXIT_SUCCESS);
}