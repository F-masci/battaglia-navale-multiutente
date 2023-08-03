#include "../lib/lib.h"
#include "../config/config.h"
#include "../config/cmd.h"

extern struct sockaddr_in addr_server;
extern int socket_client;

#define BUFF_LEN 1024
void clientConnection(void)
{

    socket_client = socket(AF_INET, SOCK_STREAM, 0);
    uint8_t cmd;
    char *buffer = (char *)malloc(sizeof(*buffer) * BUFF_LEN);
    char *nickname = NULL;
    
retry_connect:
    if(connect(socket_client, (struct sockaddr *)&addr_server, sizeof(addr_server)) != -1) {
        PRINT("\nSeleziona un comando:\n")
        PRINT("\t[1] Modifica il tuo nickname\n")
        PRINT("\t[2] Lista dei giocatori attualmente connessi\n")
        PRINT("\t[3] Inizia la partita\n\n")
client_connection_loop:
            PRINT("Comando: ")
            if(scanf("%hhu", &cmd) <= 0) {
                while((getchar()) != '\n');
                goto client_connection_loop;
            }

            bzero(buffer, BUFF_LEN);
            switch(cmd) {
                case 1: 
                    sprintf(buffer, "%hhu", CMD_SET_NICKNAME);
                    write(socket_client, buffer, strlen(buffer));
                    PRINT("Nickname: ")
                    scanf("%ms", &nickname);
                    write(socket_client, nickname, strlen(nickname));
                    PRINT("Nickname selezionato: %s\n", nickname)
                    free(nickname);
                    break;

                case 2: 
                    sprintf(buffer, "%hhu", CMD_LIST_PLAYERS);
                    write(socket_client, buffer, strlen(buffer));
                    read(socket_client, buffer, BUFF_LEN);
                    nickname = strtok(buffer, ";");
                    PRINT("\n")
                    while(nickname != NULL) {
                        PRINT("\t-> %s\n", nickname)
                        nickname = strtok(NULL, ";");
                    }
                    PRINT("\n")
                    nickname = NULL;
                    break;

                case 3: 
                    sprintf(buffer, "%hhu", CMD_START_GAME);
                    write(socket_client, buffer, strlen(buffer));
                    PRINT("In attesa degli altri giocatori...\n")
                    read(socket_client, buffer, BUFF_LEN);
                    if(strtoul(buffer, NULL, 10) == CMD_START_GAME) return;
                    break;
                default: goto client_connection_loop;
            }
            
        goto client_connection_loop;
    } else {
        sleep(2);
        goto retry_connect;
    }

}
#undef BUFF_LEN