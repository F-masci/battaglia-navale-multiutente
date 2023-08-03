#include "connection.h"

extern int socket_server;
extern struct sockaddr_in addr_server;
extern pthread_t *wthreads;

static pthread_mutex_t mut[WAITING_THREADS];

static void _sigusr1_waiting_handler(int sig, siginfo_t *dummy, void *dummy2) {
    PRINT("[SERVER]: Waiting thread (%d) exited\n", gettid())
    pthread_exit(NULL);
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

    pthread_create(&pid, NULL, client_handler, (void *) &socket_client);

    goto waiting_thread_loop;

    return NULL;

}

void waitConnections(void)
{

    /* -- SETTING SIGNAL -- */

    sigset_t set;
    sigemptyset(&set);
    
    struct sigaction sa;
    sa.sa_sigaction = _sigusr1_waiting_handler;
    sa.sa_mask = set;
    sa.sa_flags = 0;
    sa.sa_restorer = NULL;

    sigaction(SIGUSR1, &sa, NULL);

    /* -- STARTING SERVER -- */

    socket_server = socket(AF_INET, SOCK_STREAM, 0);
    setsockopt(socket_server, SOL_SOCKET, SO_REUSEADDR, &(int) {1}, sizeof(int));
    bind(socket_server, (struct sockaddr *) &addr_server, sizeof(addr_server));
    listen(socket_server, PENDING);

    /* -- START WAITING THREADS -- */

    for(size_t i=0; i<WAITING_THREADS; i++) {
        
        pthread_mutex_init(&(mut[i]), NULL);
        pthread_mutex_lock(&(mut[i]));
        
        pthread_create(wthreads+i, NULL, _waiting_thread, (void *) i);
    }

    /* -- UNLOCK FIRST THREAD -- */

    pthread_mutex_unlock(&(mut[0]));

    /* -- WAITING -- */

server_loop:
    pause();
    goto server_loop;
    
}