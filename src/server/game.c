#include "game.h"

extern uint8_t n_players;
extern player_t **players;

#define INDEX_LEN 3
void gameInitialization(void){

    size_t nick_len;
    char *encoded = (char *) malloc(n_players * NICKNAME_LEN * sizeof(char));
    bzero(encoded, sizeof(*encoded) * n_players * NICKNAME_LEN);

    char *cur = encoded;

    for(uint8_t i=0; i<n_players; i++){
        nick_len = strlen(players[i]->nickname);
        memcpy(cur, players[i]->nickname, nick_len);
        cur += nick_len;
        *cur++=';';
    }

    for(uint8_t i=0; i<n_players; i++){
        writeNum(players[i], n_players);
        writeNum(players[i], i);
        writeString(players[i], encoded);
    }
    
    return;

}
#undef INDEX_LEN

void get_move(player_t *player){

    uint8_t index, x, y;

    char *encoded_move = NULL;

    waitString(player, &encoded_move);

    index = encoded_move[0] - '0';
    x = encoded_move[1] - '0';
    y = encoded_move[2] - '0';

    if(players[index]->map->grid[y][x] == '1'){
        players[index]->map->grid[y][x] = '2';
    }

    return;
}

void send_maps(player_t *player, size_t index){

    char *encoded=(char *) malloc((n_players + (n_players * MAP_SIZE * MAP_SIZE) + 1) * sizeof(char));
    bzero(encoded, sizeof(*encoded));

    char *cur = encoded;

    for(size_t i=0; i<n_players; i++){
        if(i!=index){
            *cur++='0' + (i+1);
        }
        else *cur++='M';

        for(int j=0; j<MAP_SIZE; j++){
            for(int k=0; k<MAP_SIZE; k++){
                *cur++=players[i]->map->grid[j][k];
            }
        }
    }

    *cur='\0';

    writeString(player, encoded);
    free(encoded);
    return;
}

void send_map(player_t *player){

    char *buffer=NULL;
    char *map_encoded = NULL;
    
    waitString(player, &buffer);

    uint8_t ind = (buffer[0] - '0') - 1;
    char *cur = map_encoded;

    for(int j=0; j<MAP_SIZE; j++){
        for(int k=0; k<MAP_SIZE; k++){
            *cur++=players[ind]->map->grid[j][k];
        }
    }
    *cur = '\0';

    writeString(player, map_encoded);
    free(map_encoded);
    free(buffer);
    return;

}
