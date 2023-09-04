#include "game.h"

extern int socket_client;

char **nicknames; 
uint8_t num;         //total number of players
uint8_t me;          //index of this client

void gameInitialization(void){

    char *encoded = NULL;
    waitNum((uint32_t *) &num);
    waitNum((uint32_t *) &me);
    waitString(&encoded);

    nicknames = (char **) malloc(num * sizeof(char *));
    bzero(nicknames, num * sizeof(char *));

    char *token = NULL;
    uint8_t p = 0;

    token = strtok(encoded, ";");

    while(token != NULL && p < num){
        nicknames[p] = (char *) malloc(NICKNAME_LEN * sizeof(char));
        bzero(nicknames[p], NICKNAME_LEN * sizeof(char));
        memcpy(nicknames[p++], token, strlen(token));
        token = strtok(NULL, ";");
    }

    for(size_t i=0; i<num; i++) {
        DEBUG("[DEBUG]: %s\n", nicknames[i]);
    }

    return;

}

void make_move(void){

    char *encoded_move = (char *) malloc(4 * sizeof(char)); 
    bzero(encoded_move, 4 * sizeof(char));

    uint8_t x, y;

    char *cur = encoded_move;

    *cur++ = '0' + (choose_player() - 1);

    //mental note: check coordinates values

    PRINT("\nCoordinata x: ");
retry_x:
    if(scanf("%hhu", &x)<=0){
        while((getchar()) != '\n');
        goto retry_x;
    }

    *cur++ = '0' + x;

    PRINT("\nCoordinata y: ");
retry_y:
    if(scanf("%hhu", &y)<=0){
        while((getchar()) != '\n');
        goto retry_y;
    }

    *cur++ = '0' + y;
    *cur = '\0';

    writeString(encoded_move);

    return;

}

uint8_t choose_player(void){
    uint8_t ind;
    
    PRINT("\nScegli giocatore: \n\n");

    for(size_t i=0; i<num; i++){
        if(i==me) PRINT("\t[%ld] TU - %s\n", i+1, nicknames[i])
        else PRINT("\t[%ld] %s\n", i+1, nicknames[i]);
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

    char *encoded = NULL;
    waitString(&encoded);

    char *cur = encoded;

    for(size_t j=0; j<num; j++){
        if(*cur++ == 'M'){
            PRINT("\n[LA TUA MAPPA] %s\n", nicknames[j]);
        }
        else PRINT("\n[GIOCATORE %ld] %s\n", j+1, nicknames[j]); 

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
    
    free(encoded);

    return;

}

void print_map(void){

    char *encoded = NULL;
    
    writeNum((uint32_t) choose_player()-1);

    waitString(&encoded);

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

    free(encoded);

}