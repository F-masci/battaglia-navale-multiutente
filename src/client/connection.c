#include "../lib/lib.h"
#include "../config/config.h"

extern struct sockaddr_in addr_server;

#define BUFF_LEN 4096
void clientConnection(void)
{

    int socket_client = socket(AF_INET, SOCK_STREAM, 0);
    char *buffer = malloc(sizeof(*buffer) * BUFF_LEN);

retry_connect:
    if(connect(socket_client, (struct sockaddr *)&addr_server, sizeof(addr_server)) != -1) {
        do
        {
            read(0, buffer, BUFF_LEN);
            write(socket_client, buffer, strlen(buffer));
        } while (strcmp(buffer, "quit") != 0);
        close(socket_client);
    } else {
        sleep(2);
        goto retry_connect;
    }
}
#undef BUFF_LEN