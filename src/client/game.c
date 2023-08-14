#include "game.h"

extern socket_client;

char **nicknames; 
size_t num; //total number of players
size_t me; //index of this client

#define BUFF_LEN 1024
void gameInitialization(void){

    char *encoded = (char *) malloc(BUFF_LEN * sizeof(char));
    bzero(encoded, sizeof(*encoded));

    read(socket_client, encoded, BUFF_LEN);
    
    num = encoded[0] - '0'; 

    nicknames = (char **) malloc(num * sizeof(char *));
    for(int i=0; i<num; i++){
        nicknames[i] = (char *) malloc(255 * sizeof(char));
    }

    char *cur = encoded+1;
    char *token;
    int p = 0;
    int len=0;

    token=strtok(cur, ";");

    while(token != NULL && p<num){
        strncpy(nicknames[p++], token, 255);
        len+=strlen(nicknames[p-1]);
        token=strtok(NULL, ";");
    }

    cur = encoded + len + num + 1;
    me = *cur - '0';

    return;

}

uint8_t choose_player(void){
    uint8_t ind;
    
    PRINT("\nScegli giocatore: \n\n");

    for(size_t i=0; i<num; i++){
        if(i==me) PRINT("\t[%d] TU - %s\n", i+1, nicknames[i])
        else PRINT("\t[%d] %s\n", i+1, nicknames[i]);
    }

    PRINT("\nGiocatore: ");
retry:
    if(scanf("%hhu", &ind)<=0){
        while((getchar()) != '\n');
        goto retry;
    }

    return ind;

}

void print_maps(void){

    char *encoded=(char *) malloc(sizeof(char) * BUFF_LEN);
    bzero(encoded, BUFF_LEN);

    read(socket_client, encoded, BUFF_LEN);

    char *cur = encoded;

    for(size_t j=0; j<num; j++){
        if(*cur++ == 'M'){
            PRINT("\n[LA TUA MAPPA] %s\n", nicknames[j]);
        }
        else PRINT("\n[GIOCATORE %d] %s\n", j+1, nicknames[j]); 

        PRINT("\n    ");
        for(int i=0; i<MAP_SIZE; i++){
            PRINT(" %d  ", i);
        }
        PRINT("\n    ");
        for(int i=0; i<MAP_SIZE; i++){
            PRINT("----");
        }
        PRINT("\n");
        for(int i=0; i<MAP_SIZE; i++){
            PRINT(" %d ", i);
            for(int k=0; k<MAP_SIZE; k++) {
                switch(*cur++){
                    case '0':
                        PRINT("|   ");
                        break;
                    case '1':
                        PRINT("| X ");
                        break;
                    case '2':
                        PRINT("| K ");
                        break;
                    default: break;
                }
            }
            PRINT("|\n");
        }
        PRINT("    ");
        for(int i=0; i<MAP_SIZE; i++){
            PRINT("----");
        }
        PRINT("\n");
    }
    PRINT("\n");
        
    return;

}

void print_map(void){

    char *encoded = (char *) malloc(sizeof(char) * BUFF_LEN);
    bzero(encoded, sizeof(*encoded));
    char ind;

    ind='0' + choose_player();
    write(socket_client, &ind, 1);

    read(socket_client, encoded, BUFF_LEN);
    PRINT("%s\n", encoded);

    char *cur = encoded;

    PRINT("\n    ");
    for(int i=0; i<MAP_SIZE; i++){
        PRINT(" %d  ", i)
    }
    PRINT("\n    ");
    for(int i=0; i<MAP_SIZE; i++){
        PRINT("----");
    }
    PRINT("\n");
    for(int i=0; i<MAP_SIZE; i++){
        PRINT(" %d ", i)
        for(int k=0; k<MAP_SIZE; k++) {
            switch(*cur++){
                case '0':
                    PRINT("|   ")
                    break;
                case '1':
                    PRINT("| X ")
                    break;
                case '2':
                    PRINT("| K ")
                    break;
                default: break;
            }
        }
        PRINT("|\n");
    }
    PRINT("    ");
    for(int i=0; i<MAP_SIZE; i++){
        PRINT("----")
    }
    PRINT("\n")

}
#undef BUFF_LEN