#include "connection.h"

extern int socket_server;
extern struct sockaddr_in addr_server;
extern pthread_t *w_threads;

extern size_t n_players;                                // Numero di giocatori in lobby
extern player_t **players;                              // Array di giocatori

static pthread_mutex_t mut[WAITING_THREADS];

static void _sigusr1_waiting_handler(int sig, siginfo_t *dummy, void *dummy2) {
    PRINT("[SERVER]: Waiting thread (%d) exited\n", gettid())
    pthread_exit(NULL);
}

static void _sigusr2_waiting_handler(int sig, siginfo_t *dummy, void *dummy2) {
    PRINT("[SERVER]: Waiting connection termination check (%d)\n", gettid())
}

static void *_waiting_thread(void * args) {

    size_t index = (size_t) args;
    size_t next = (index+1)%WAITING_THREADS;

    pthread_t pid;

    int socket_client;
    struct sockaddr_in addr_client;
    socklen_t len_client = sizeof(addr_client);

waiting_thread_loop:

    pthread_mutex_lock(&(mut[index]));
    PRINT("[THREAD %ld]: ready to accept\n", index)
    while ((socket_client = accept(socket_server, (struct sockaddr *) &addr_client, &len_client)) == -1);
    PRINT("[THREAD %ld]: accepted\n", index)
    pthread_mutex_unlock(&(mut[next]));

    pthread_create(&pid, NULL, clientHandler, (void *) &socket_client);

    goto waiting_thread_loop;

    return NULL;

}

void waitConnections(void)
{

    /* -- SETTING SIGNAL -- */
    /**
     * La maschera dei segnali viene impostata sul thread corrente per bloccare tutti i segnali in ingresso eccetto SIGUSR1 e SIGINT.
     * Questa maschera verrà ereditata da tutti i waiting threads.
     * La mscherà per il thread corrente verrà modificata per bloccare SIGUSR2 invece di SIGUSR1.
     * Il segnale SIGUSR1 interrompe i waiting threads mentre il segnale SIGUSR2 terminerà la funzione waitConnections e ritornerà
     * il controllo al main per il gioco vero e prorpio.
    */

    sigset_t set;
    sigfillset(&set);
    sigdelset(&set, SIGUSR1);
    sigdelset(&set, SIGINT);
    sigprocmask(SIG_SETMASK, &set, NULL);
    
    struct sigaction sa;
    bzero(&sa, sizeof(sa));
    sa.sa_sigaction = _sigusr1_waiting_handler;
    sa.sa_mask = set;
    sa.sa_flags = 0;
    sa.sa_restorer = NULL;
    sigaction(SIGUSR1, &sa, NULL);

    sigemptyset(&set);
    bzero(&sa, sizeof(sa));
    sa.sa_sigaction = _sigusr2_waiting_handler;
    sa.sa_mask = set;
    sa.sa_flags = 0;
    sa.sa_restorer = NULL;
    sigaction(SIGUSR2, &sa, NULL);

    /* -- STARTING SERVER -- */

    socket_server = socket(AF_INET, SOCK_STREAM, 0);
    setsockopt(socket_server, SOL_SOCKET, SO_REUSEADDR, &(int) {1}, sizeof(int));
    bind(socket_server, (struct sockaddr *) &addr_server, sizeof(addr_server));
    listen(socket_server, PENDING);

    /* -- START WAITING THREADS -- */

    for(size_t i=0; i<WAITING_THREADS; i++) {
        
        pthread_mutex_init(&(mut[i]), NULL);
        pthread_mutex_lock(&(mut[i]));
        
        pthread_create(w_threads+i, NULL, _waiting_thread, (void *) i);
    }

    /* -- UNLOCK FIRST THREAD -- */

    pthread_mutex_unlock(&(mut[0]));

    /* -- SETTING SIGNAL MASK -- */

    sigfillset(&set);
    sigdelset(&set, SIGUSR2);
    sigdelset(&set, SIGINT);
    sigprocmask(SIG_SETMASK, &set, NULL);
    
    /* -- WAITING -- */
    
waiting_loop:
    pause();
    for(size_t i=0; i<n_players; i++) {
        if(players[i]->ready == false) goto waiting_loop;
    }
    PRINT("[SERVER]: Waiting connection terminated\n")

}