#include "connection.h"

pthread_mutex_t mut[WAITING_THREADS];
pthread_t *wthreads;        // Waiting threads
pthread_t *hthreads;        // Handler threads

void *waiting_thread(void * args) {

    size_t index = (size_t) args;
    size_t next = (index+1)%WAITING_THREADS;

    pthread_t pid;

    int socket_client;
    struct sockaddr_in addr_client;
    socklen_t len_client = sizeof(addr_client);

waiting_thread_loop:

    pthread_mutex_lock(&(mut[index]));
    PRINT("Thread %ld: ready to accept\n", index)
    while ((socket_client = accept(socket_server, (struct sockaddr *) &addr_client, &len_client)) == -1);
    PRINT("Thread %ld: accepted\n", index)
    pthread_mutex_unlock(&(mut[next]));

    pthread_create(&pid, NULL, client_handler, (void *) &socket_client);

    goto waiting_thread_loop;

    return NULL;

}

void waitConnections(void)
{

    wthreads = (pthread_t *) malloc(sizeof(*wthreads) * WAITING_THREADS);
    hthreads = (pthread_t *) malloc(sizeof(*hthreads) * 256);

    /* -- STARTING SERVER -- */

    socket_server = socket(AF_INET, SOCK_STREAM, 0);
    setsockopt(socket_server, SOL_SOCKET, SO_REUSEADDR, &(int) {1}, sizeof(int));
    bind(socket_server, (struct sockaddr *) &addr_server, sizeof(addr_server));
    listen(socket_server, PENDING);

    /* -- START WAITING THREADS -- */

    for(size_t i=0; i<WAITING_THREADS; i++) {
        
        pthread_mutex_init(&(mut[i]), NULL);
        pthread_mutex_lock(&(mut[i]));
        
        pthread_create(wthreads+i, NULL, waiting_thread, (void *) i);
    }

    pthread_mutex_unlock(&(mut[0]));

    sigset_t set;
    sigemptyset(&set);
    sigaddset(&set, SIGINT);
    sigprocmask(SIG_SETMASK, &set, NULL);

    /* -- WAITING -- */
server_loop:
    pause();
    goto server_loop;
    
}