#include "connection.h"

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
        PRINT("\nSeleziona un comando:\n\n")
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
                    sendCmd(CMD_SET_NICKNAME);
                    PRINT("Nickname: ")
                    scanf("%ms", &nickname);
                    write(socket_client, nickname, strlen(nickname));
                    PRINT("Nickname selezionato: %s\n", nickname)
                    free(nickname);
                    break;

                case 2: 
                    sendCmd(CMD_LIST_PLAYERS);
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
                    sendCmd(CMD_START_GAME);
                    PRINT("In attesa degli altri giocatori...\n")
                    cmd = waitCmd();
                    if(cmd == CMD_START_GAME) return;
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