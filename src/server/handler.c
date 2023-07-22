#include "../lib/lib.h"
#include "../config/config.h"

#define BUFF_LEN 4096
void *client_thread(void *args) {

    int socket_client = *( (int *) args );

    char *buffer = (char *)malloc(sizeof(*buffer) * BUFF_LEN);

    PRINT("Ready to receive data\n")

    do {
        if(read(socket_client, buffer, BUFF_LEN) <= 0) break;
        read(socket_client, buffer, BUFF_LEN);
        printf("Client: %s", buffer);
        bzero(buffer, strlen(buffer));
    } while (strcmp(buffer, "quit") != 0 );

    close(socket_client);
    free(buffer);

    return NULL;

}
#undef BUFF_LEN