#include "game.h"

extern size_t n_players;
extern player_t **players;

void send_maps(player_t *player, size_t index){

    char *encoded=(char *) malloc((n_players + (n_players * MAP_SIZE * MAP_SIZE) + (n_players * 255) + 2) * sizeof(char));
    bzero(encoded, sizeof(*encoded));

    char *cur = encoded;
    *cur++='0' + n_players;

    for(size_t i=0; i<n_players; i++){
        for(int t=0; t<strlen(players[i]->nickname); t++){
            *cur++=players[i]->nickname[t];
        }
        *cur++=';';
    }

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
    return;
}