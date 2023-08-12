#include "game.h"

extern socket_client;

char **nicknames; 

#define BUFF_LEN 1024
void print_maps(void){

    char *encoded=(char *) malloc(sizeof(char) * BUFF_LEN);
    bzero(encoded, BUFF_LEN);

    read(socket_client, encoded, BUFF_LEN);

    size_t n = encoded[0] - '0'; //total number of players

    nicknames = (char **) malloc(n * sizeof(char *));
    for(int i=0; i<n; i++){
        nicknames[i] = (char *) malloc(255 * sizeof(char));
    }

    char *cur = encoded+1;
    char *token;
    int p = 0;
    int len=0;

    token=strtok(cur, ";");

    while(token != NULL && p<n){
        strncpy(nicknames[p++], token, 255);
        len+=strlen(nicknames[p-1]);
        token=strtok(NULL, ";");
    }

    cur = encoded + len + n + 1;

    for(size_t j=0; j<n; j++){
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
#undef BUFF_LEN