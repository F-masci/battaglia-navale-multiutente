#include "connection.h"

extern int socket_server;
extern struct sockaddr_in addr_server;
extern pthread_t *w_threads;

extern uint8_t n_players;                                // Numero di giocatori in lobby
extern player_t **players;                               // Array di giocatori

static pthread_mutex_t mut[WAITING_THREADS];

static bool stop_server = false;

static void _sigusr1_waiting_handler(int sig, siginfo_t *dummy, void *dummy2) {
    DEBUG("[SERVER]: Waiting thread (%d) exited\n", gettid())
    pthread_exit(NULL);
}

static void _sigusr2_waiting_handler(int sig, siginfo_t *dummy, void *dummy2) {
    DEBUG("[SERVER]: Waiting connection termination check (%d)\n", gettid())
    if(n_players <= 0) return;
    for(size_t i=0; i<n_players; i++) {
        if(players[i]->ready == false) return;
    }
    DEBUG("[SERVER]: Waiting connection terminated\n")
    stop_server = true;
}

static void *_waiting_thread(void * args) {

    errno = 0;

    size_t index = (size_t) args;
    size_t next = (index+1)%WAITING_THREADS;

    pthread_t pid;

    int socket_client;
    struct sockaddr_in addr_client;
    socklen_t len_client = sizeof(addr_client);

waiting_thread_loop:

    if(!pthread_mutex_lock(&(mut[index]))) EXIT_ERRNO
    PRINT("[THREAD %ld]: ready to accept\n", index)
    while ((socket_client = accept(socket_server, (struct sockaddr *) &addr_client, &len_client)) == -1) EXIT_ERRNO
    PRINT("[THREAD %ld]: accepted\n", index)
    if(!pthread_mutex_unlock(&(mut[next]))) EXIT_ERRNO;

    if(!pthread_create(&pid, NULL, clientHandler, (void *) &socket_client)) EXIT_ERRNO

    goto waiting_thread_loop;

    return NULL;

}

void waitConnections(void)
{

    /* -- SETTING SIGNAL -- */
    /**
     * La maschera dei segnali viene impostata sul thread corrente per bloccare tutti i segnali in ingresso eccetto SIGUSR1 e SIGINT.
     * Questa maschera verrà ereditata da tutti i waiting threads.
     * La maschera per il thread corrente verrà modificata per bloccare SIGUSR2 invece di SIGUSR1.
     * Il segnale SIGUSR1 interrompe i waiting threads mentre il segnale SIGUSR2 terminerà la funzione waitConnections e ritornerà
     * il controllo al main per il gioco vero e proprio.
    */

    sigset_t set;
    sigemptyset(&set);
    sigaddset(&set, SIGUSR1);
    if(sigprocmask(SIG_UNBLOCK, &set, NULL) == -1) EXIT_ERRNO
    
    sigemptyset(&set);

    struct sigaction sa;
    BZERO(&sa, sizeof(sa));
    sa.sa_sigaction = _sigusr1_waiting_handler;
    sa.sa_mask = set;
    sa.sa_flags = 0;
    sa.sa_restorer = NULL;
    if(sigaction(SIGUSR1, &sa, NULL) == -1) EXIT_ERRNO

    BZERO(&sa, sizeof(sa));
    sa.sa_sigaction = _sigusr2_waiting_handler;
    sa.sa_mask = set;
    sa.sa_flags = 0;
    sa.sa_restorer = NULL;
    if(sigaction(SIGUSR2, &sa, NULL) == -1) EXIT_ERRNO

    /* -- STARTING SERVER -- */

    if( (socket_server = socket(AF_INET, SOCK_STREAM, 0)) == -1) EXIT_ERRNO;
    if(setsockopt(socket_server, SOL_SOCKET, SO_REUSEADDR, &(int) {1}, sizeof(int)) == -1) EXIT_ERRNO;
    if(bind(socket_server, (struct sockaddr *) &addr_server, sizeof(addr_server)) == -1) EXIT_ERRNO
    if(listen(socket_server, PENDING) == -1) EXIT_ERRNO;

    /* -- START WAITING THREADS -- */

    for(size_t i=0; i<WAITING_THREADS; i++) {
        
        pthread_mutex_init(&(mut[i]), NULL);
        if(!pthread_mutex_lock(&(mut[i]))) EXIT_ERRNO
        
        if(!pthread_create(w_threads+i, NULL, _waiting_thread, (void *) i)) EXIT_ERRNO
    }

    /* -- UNLOCK FIRST THREAD -- */

    if(!pthread_mutex_unlock(&(mut[0]))) EXIT_ERRNO

    /* -- SETTING SIGNAL MASK -- */

    sigemptyset(&set);
    sigaddset(&set, SIGUSR2);
    if(sigprocmask(SIG_UNBLOCK, &set, NULL) == -1) EXIT_ERRNO

    sigemptyset(&set);
    sigaddset(&set, SIGUSR1);
    if(sigprocmask(SIG_BLOCK, &set, NULL) == -1) EXIT_ERRNO
    
    /* -- LOCAL CONNECTION -- */
    /**
     * Il thread attende un qualsiasi pacchetto UDP sulla porta 6501.
     * Risponde di conseguenza sulla porta 6502 per notificare che è un server di battaglia navale
    */

    struct sockaddr_in local_server_addr;

    BZERO((char*) &local_server_addr, sizeof(local_server_addr));
    local_server_addr.sin_family = AF_INET;
    local_server_addr.sin_port = htons(UDP_PORT_SRV);     // 6501
    local_server_addr.sin_addr.s_addr = ADDRESS;          // 0.0.0.0

    int local_server_socket = socket(AF_INET, SOCK_DGRAM, 0);
    if(local_server_socket == -1) EXIT_ERRNO
    if(setsockopt(local_server_socket, SOL_SOCKET, SO_REUSEADDR, &(int) {1}, sizeof(int)) == -1) EXIT_ERRNO
    if(bind(local_server_socket, (struct sockaddr *) &local_server_addr, sizeof(local_server_addr)) == -1) EXIT_ERRNO
    
    struct sockaddr_in client_addr;
    socklen_t client_addr_len = sizeof(client_addr);
    BZERO((char *) &client_addr, sizeof(client_addr));

local_connection_loop:

    if(stop_server) {
        while(close(local_server_socket) == -1) EXIT_ERRNO
        return;
    }
    if(recvfrom(local_server_socket, NULL, 0, MSG_TRUNC, (struct sockaddr *) &client_addr, &client_addr_len) == -1) {
        EXIT_ERRNO
        goto local_connection_loop;
    }
    DEBUG("[DEBUG]: received request from %s (port %d)\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port))
    client_addr.sin_port = htons(UDP_PORT_CLN);     // 6502
    while(sendto(local_server_socket, NULL, 0, 0, (struct sockaddr *) &client_addr, sizeof(client_addr)) == -1) EXIT_ERRNO

    goto local_connection_loop;

}