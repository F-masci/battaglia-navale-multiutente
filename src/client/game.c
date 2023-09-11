#include "game.h"

extern int socket_client;

extern char **nicknames; 
extern uint8_t num;                     //total number of players
extern uint8_t me;                      //index of this client

void gameInitialization(void){

    errno = 0;

    char *encoded = NULL;
    if(!waitNum((uint32_t *) &num)) EXIT_ERRNO
    if(!waitNum((uint32_t *) &me)) EXIT_ERRNO
    if(!waitString(&encoded)) EXIT_ERRNO

    nicknames = (char **) malloc(num * sizeof(char *));
    if(nicknames == NULL) EXIT_ERRNO
    
    BZERO(nicknames, num * sizeof(char *));

    char *token = NULL;
    uint8_t p = 0;

    token = strtok(encoded, ";");

    while(token != NULL && p < num){
        nicknames[p] = (char *) malloc(NICKNAME_LEN * sizeof(char));
        if(nicknames[p] == NULL) EXIT_ERRNO
        BZERO(nicknames[p], NICKNAME_LEN * sizeof(char));
        memcpy(nicknames[p++], token, strlen(token));
        token = strtok(NULL, ";");
    }

    for(uint8_t i=0; i<num; i++) {
        DEBUG("[DEBUG]: %s\n", nicknames[i]);
    }

    return;

}

#define ENCODED_MOVE_LEN 4
/**
 * @brief 
 * 
 */
void makeMove(void){

    errno = 0;

    char *encoded_move = (char *) malloc(ENCODED_MOVE_LEN * sizeof(*encoded_move));
    if(encoded_move == NULL) EXIT_ERRNO
    BZERO(encoded_move, ENCODED_MOVE_LEN * sizeof(*encoded_move));

    cell_t x, y;
    char *cur = encoded_move;

    *cur++ = '0' + choosePlayer(false);

    //FIXME: check coordinates values

make_move_retry:

    PRINT("Scegli cella [es. A 0]: ");

    if(scanf("%c %hhu", &x, &y) <=1 ) {
        EXIT_ERRNO
        while((getchar()) != '\n');
        goto make_move_retry;
    }
    while((getchar()) != '\n');
    
    if( (x < 'A' || x > 'J') && (x < 'a' || x > 'j')) goto make_move_retry;

    x = toupper(x) - 'A';
    
    if(x > 9 || y > 9) goto make_move_retry;

    *cur++ = '0' + x;
    *cur++ = '0' + y;

    if(!writeString(encoded_move)) EXIT_ERRNO

}
#undef ENCODED_MOVE_LEN

void printMaps(void) {

    char *encoded = NULL;
    if(!waitString(&encoded)) EXIT_ERRNO

    char *cur = encoded;

    for(uint8_t j=0; j<num; j++){
        cur = encoded + j * (MAP_SIZE * MAP_SIZE + 1);
        if(*cur == 'M'){
            PRINT("\n[LA TUA MAPPA] %s\n", nicknames[j]);
            cur = encoded + j * (MAP_SIZE * MAP_SIZE + 1) + 1;
            printMap(cur, true);
        } else {
            PRINT("\n[GIOCATORE %hhu] %s\n", j+1, nicknames[j]);
            cur = encoded + j * (MAP_SIZE * MAP_SIZE + 1) + 1;
            printMap(cur, false);
        }
    }
    PRINT("\n");
    
    free(encoded);
    encoded = NULL;

}

void printMap(const char *encoded, bool show_all) {

    PRINT("\n    ");
    for(uint8_t i=0; i<MAP_SIZE; i++){
        PRINT(" %c  ", 'A' + i);
    }
    PRINT("\n    ");
    for(uint8_t i=0; i<MAP_SIZE; i++){
        PRINT("----");
    }
    PRINT("\n");

    for(uint8_t i=0; i<MAP_SIZE; i++){
        PRINT(" %d ", i);
        for(uint8_t k=0; k<MAP_SIZE; k++) {
            switch(*encoded++){
                case '0':
                    PRINT("|   ");
                    break;
                case '1':
                    PRINT(show_all ? "| X " : "|   ");  // Con show_all impostato a true verranno mostrate anche le navi non ancora colpite
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

    PRINT("    ");
    for(int i=0; i<MAP_SIZE; i++){
        PRINT("----");
    }
    PRINT("\n");

}

uint8_t choosePlayer(const bool _insert_me) {

    uint16_t ind;
    
    PRINT("\nScegli giocatore: \n\n");

    for(uint16_t i=0; i<num; i++){
        if(i == me && _insert_me) PRINT("\t[%hu] TU - %s\n", i+1, nicknames[i])
        else if(i != me) PRINT("\t[%hu] %s\n", i+1, nicknames[i]);
    }

choose_player_retry:

    PRINT("\nGiocatore: ");

    if(scanf("%hu", &ind) <=0 ) {
        EXIT_ERRNO
        while((getchar()) != '\n');
        goto choose_player_retry;
    }
    while((getchar()) != '\n');

    if(ind <= 0 || ind > num) goto choose_player_retry;
    if(!_insert_me && ind-1 == me) goto choose_player_retry;

    return (uint8_t) ind-1;

}