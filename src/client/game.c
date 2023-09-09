#include "game.h"

extern int socket_client;

char **nicknames; 
extern uint8_t num;         //total number of players
uint8_t me;          //index of this client

void gameInitialization(uint8_t time){

    char *encoded = NULL;
    waitNum((uint32_t *) &num);
    waitNum((uint32_t *) &me);
    waitString(&encoded);

    DEBUG("check 1\n");

    if(time == 0) nicknames = (char **) malloc(num * sizeof(char *));
    else nicknames = realloc(nicknames, num * sizeof(char *));

    DEBUG("check 2\n");
    
    BZERO(nicknames, num * sizeof(char *));

    char *token = NULL;
    uint8_t p = 0;

    token = strtok(encoded, ";");

    while(token != NULL && p < num){
        if(time == 0) nicknames[p] = (char *) malloc(NICKNAME_LEN * sizeof(char));
        else nicknames[p] = realloc(nicknames + (p * NICKNAME_LEN), NICKNAME_LEN * sizeof(char));   //sistemare errore invalid pointer
        DEBUG("check 3\n");
        BZERO(nicknames[p], NICKNAME_LEN * sizeof(char));
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
    BZERO(encoded_move, 4 * sizeof(char));

    uint8_t x, y;

    char *cur = encoded_move;

    *cur++ = '0' + (choose_player(1) - 1);

    //mental note: check coordinates values

    PRINT("Coordinata x: ");
retry_x:
    if(scanf("%hhu", &x)<=0){
        while((getchar()) != '\n');
        goto retry_x;
    }

    *cur++ = '0' + x;

    PRINT("Coordinata y: ");
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

uint8_t choose_player(int option){
    uint8_t ind;
    
    PRINT("\nScegli giocatore: \n\n");

    for(size_t i=0; i<num; i++){
        if(i==me && option==0) PRINT("\t[%ld] TU - %s\n", i+1, nicknames[i])
        else if(i!=me) PRINT("\t[%ld] %s\n", i+1, nicknames[i]);
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
        cur = encoded + (j * MAP_SIZE * MAP_SIZE) + j;
        if(*cur == 'M'){
            PRINT("\n[LA TUA MAPPA] %s\n", nicknames[j]);
            cur = encoded + (j * MAP_SIZE * MAP_SIZE) + j + 1;
            map_print(0, cur);
        }
        else{
            PRINT("\n[GIOCATORE %ld] %s\n", j+1, nicknames[j]);
            cur = encoded + (j * MAP_SIZE * MAP_SIZE) + j + 1; 
            map_print(1, cur);
        }
    }
    PRINT("\n");
    
    free(encoded);

    return;

}

void print_map(void){

    char *encoded = NULL;
    uint8_t p = choose_player(0) - 1;
    
    writeNum((uint32_t) p);

    waitString(&encoded);

    if(p == me) map_print(0, encoded);
    else map_print(1, encoded);

    free(encoded);

    return;

}

void map_print(int option, char *encoded){

    char *cur = encoded;

    PRINT("\n    ");
    for(int i=0; i<MAP_SIZE; i++){
        PRINT(" %d  ", i);
    }
    PRINT("\n    ");
    for(int i=0; i<MAP_SIZE; i++){
        PRINT("----");
    }
    PRINT("\n");

    if(option==0){
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
                    case '3':
                        PRINT("| F ");
                        break;
                    default: break;
                }
            }
            PRINT("|\n");
        }
    }
    else{
        for(int i=0; i<MAP_SIZE; i++){
            PRINT(" %d ", i);
            for(int k=0; k<MAP_SIZE; k++) {
                switch(*cur++){
                    case '0':
                        PRINT("|   ");
                        break;
                    case '1':
                        PRINT("|   ");
                        break;
                    case '2':
                        PRINT("| K ");
                        break;
                    case '3':
                        PRINT("| F ");
                        break;
                    default: break;
                }
            }
            PRINT("|\n");
        }
    }

    PRINT("    ");
    for(int i=0; i<MAP_SIZE; i++){
        PRINT("----");
    }
    PRINT("\n");

    return;
}