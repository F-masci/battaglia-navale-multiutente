#include "connection.h"

extern struct sockaddr_in addr_server;
extern int socket_client;

void clientConnection(void)
{

    errno = 0;

    while( (socket_client = socket(AF_INET, SOCK_STREAM, 0)) == -1) EXIT_ERRNO
    uint8_t cmd;
    char *buffer = NULL;
    char *nickname = (char *) malloc(NICKNAME_LEN * sizeof(*nickname));
    
    while(connect(socket_client, (struct sockaddr *)&addr_server, sizeof(addr_server)) == -1) EXIT_ERRNO

    PRINT("\nSeleziona un comando:\n\n")
    PRINT("\t[1] Modifica il tuo nickname\n")
    PRINT("\t[2] Lista dei giocatori attualmente connessi\n")
    PRINT("\t[3] Inizia la partita\n")
    PRINT("\t[4] Disconnettiti\n\n")

client_connection_loop:
    PRINT("Comando: ")
    if(scanf("%hhu", &cmd) <= 0) {
        EXIT_ERRNO
        while((getchar()) != '\n');
        goto client_connection_loop;
    }
    while((getchar()) != '\n');

    switch(cmd) 
    {
        case 1: 
            if(nickname == NULL) EXIT_ERRNO
            BZERO(nickname, sizeof(*nickname) * NICKNAME_LEN);
            if(!sendCmd(CMD_SET_NICKNAME)) EXIT_ERRNO
            PRINT("Nickname (non sono ammessi spazi): ")
            while(scanf("%ms", &nickname) <= 0) {
                EXIT_ERRNO
                while((getchar()) != '\n');
            }
            while((getchar()) != '\n');
            if(!writeString(nickname)) EXIT_ERRNO
            PRINT("Nickname selezionato: %s\n", nickname)
            break;

        case 2: 
            if(!sendCmd(CMD_LIST_PLAYERS)) EXIT_ERRNO
            if(!waitString(&buffer)) EXIT_ERRNO
            nickname = strtok(buffer, ";");
            PRINT("\n")
            while(nickname != NULL) {
                PRINT("\t-> %s\n", nickname)
                nickname = strtok(NULL, ";");
            }
            PRINT("\n")
            nickname = NULL;
            free(buffer);
            break;

        case 3: 
            if(!sendCmd(CMD_START_GAME)) EXIT_ERRNO
            free(nickname);
            nickname = NULL;
            PRINT("In attesa degli altri giocatori...\n")
            cmd_t rec_cmd = waitCmd();
            if(rec_cmd == CMD_START_GAME) return;
            if(rec_cmd == CMD_ERROR) EXIT_ERRNO
            if(rec_cmd == CMD_CLOSE_CONNECTION) {
                while(close(socket_client) == -1) EXIT_ERRNO
                exit(EXIT_SUCCESS);
            }
            break;

        case 4: 
            if(!sendCmd(CMD_CLOSE_CONNECTION)) EXIT_ERRNO
            while(close(socket_client) == -1) EXIT_ERRNO
            exit(EXIT_SUCCESS);
            break;

        default: goto client_connection_loop;
    }
        
    goto client_connection_loop;
}