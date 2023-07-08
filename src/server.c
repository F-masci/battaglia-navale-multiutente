#include "lib.h"
#include "config.h"

extern int socket_server;
extern struct sockaddr_in addr_server;

pthread_mutex_t mut[WAITING_THREADS];

#define BUFF_LEN 4096
void *client_thread(void *args) {

    int socket_client = *( (int *) args );

    char *buffer = (char *)malloc(sizeof(*buffer) * BUFF_LEN);

    PRINT("Ready to receive data\n")

    do {
        read(socket_client, buffer, BUFF_LEN);
        printf("Client: %s", buffer);
        bzero(buffer, strlen(buffer));
    } while (strcmp(buffer, "quit") != 0);
    close(socket_client);

    free(buffer);

    return NULL;

}
#undef BUFF_LEN


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

    pthread_create(&pid, NULL, client_thread, (void *) &socket_client);

    goto waiting_thread_loop;

    return NULL;

}

void waitConnections(void)
{

    pthread_t pid;

    /* -- STARTING SERVER -- */

    socket_server = socket(AF_INET, SOCK_STREAM, 0);
    bind(socket_server, (struct sockaddr *) &addr_server, sizeof(addr_server));
    listen(socket_server, PENDING);

    /* -- START WAITING THREADS -- */

    for(size_t i=0; i<WAITING_THREADS; i++) {
        
        pthread_mutex_init(&(mut[i]), NULL);
        pthread_mutex_lock(&(mut[i]));
        
        pthread_create(&pid, NULL, waiting_thread, (void *) i);
    }

    pthread_mutex_unlock(&(mut[0]));

    /* -- WAITING -- */
server_loop:
    pause();
    goto server_loop;
    
}