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

    /* -- INIT GLOBAL VARS -- */

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

    /* -- WAITING FOR ALL MAPS -- */

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

    /* -- GAME -- */

    gameInitialization();               //INITIALIZATION

    PRINT("[SERVER] Game initialization done\n")

    size_t index = 0;                   // Indice del giocatore di turno
    cmd_t cmd;                          // Comando ricevuto

    PRINT("[SERVER] Starting game\n")

main_loop:

    // Start player turn
    if(!sendCmd(players[index], CMD_TURN)) {
        CHECK_ERRNO("Error")
        goto main_exit;
    }

main_cmd_loop:
    cmd = waitCmd(players[index]);
    PRINT("[%s]: request command %hu\n", players[index]->nickname, cmd)
    switch(cmd) {

        case CMD_GET_MAPS: 
            sendMaps(players[index]);
            goto main_cmd_loop;

        case CMD_GET_MAP:
            sendMap(players[index]);
            goto main_cmd_loop;

        case CMD_MOVE:
            getMove(players[index]);   // FIXME: se viene eliminato un giocatore l'indice potrebbe saltare il turno di un giocatore
            break;

        case CMD_CLOSE_CONNECTION:
            goto main_exit;

        case CMD_ERROR:
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