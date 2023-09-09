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

#define BUFF_LEN 1024
void get_move(player_t *player, int ind){

    uint8_t index, x1, y1;
    int i;
    char *message = (char *) malloc(BUFF_LEN * sizeof(char));
    bzero(message, BUFF_LEN);

    char *encoded_move = NULL;

    waitString(player, &encoded_move);

    index = encoded_move[0] - '0';
    x1 = encoded_move[1] - '0';
    y1 = encoded_move[2] - '0';

    if(players[index]->map->grid[y1][x1] == '1'){
        players[index]->map->grid[y1][x1] = '2';
        sprintf(message, "%s ha colpito %s [%hhu; %hhu]\n", players[ind]->nickname, players[index]->nickname, x1, y1);
    }
    else if(players[index]->map->grid[y1][x1] == '0' || players[index]->map->grid[y1][x1] == '3'){
        players[index]->map->grid[y1][x1] = '3';
        sprintf(message, "%s ha mancato %s [%hhu; %hhu]\n", players[ind]->nickname, players[index]->nickname, x1, y1);
    }
    else if(players[index]->map->grid[y1][x1] == '2'){
        sprintf(message, "%s ha colpito una parte di una nave di %s già colpita [%hhu; %hhu]\n", players[ind]->nickname, players[index]->nickname, x1, y1);
    }

    for(int h=0; h<n_players; h++){
        sendCmd(players[h], CMD_STATUS);
        writeString(players[h], message);
    }

    //il server esegue il controllo 
    int x, y, dim;
    
    bzero(message, BUFF_LEN);

    //aggiorna 
    for(int k=0; k<SHIPS_NUM; k++){
        if(players[index]->map->ships[k].sunk == false){
            
            x = players[index]->map->ships[k].x;
            y = players[index]->map->ships[k].y;
            dim = players[index]->map->ships[k].dim;

            switch(players[index]->map->ships[k].dir){
                case 'W':
                    for(i=y; i>y-dim; i--){
                        if(players[index]->map->grid[i][x] =='1') goto next;
                    }
                    players[index]->map->ships[k].sunk = true;
                    break;
                case 'A':
                    for(i=x; i>x-dim; i--){
                        if(players[index]->map->grid[y][i] =='1') goto next;
                    }
                    players[index]->map->ships[k].sunk = true;
                    break;
                case 'S':
                    for(i=y; i<y+dim; i++){
                        if(players[index]->map->grid[i][x] =='1') goto next;
                    }
                    players[index]->map->ships[k].sunk = true;
                    break;
                case 'D':
                    for(i=x; i<x+dim; i++){
                        if(players[index]->map->grid[y][i] =='1') goto next;
                    }
                    players[index]->map->ships[k].sunk = true;
                    break;
                default: break;
            }
        }

        switch(dim){
            case 2:
                sprintf(message, "%s ha affondato DESTROYER di %s\n", players[ind]->nickname, players[index]->nickname);
                break;
            case 3:
                sprintf(message, "%s ha affondato SUBMARINE di %s\n", players[ind]->nickname, players[index]->nickname);
                break;
            case 4:
                sprintf(message, "%s ha affondato BATTLESHIP di %s\n", players[ind]->nickname, players[index]->nickname);
                break;
            case 5:
                sprintf(message, "%s ha affondato CARRIER di %s\n", players[ind]->nickname, players[index]->nickname);
                break;
            default: break;
        }

        goto send;

        next:
        ;
    }

    sprintf(message, "%s non ha affondato nessuna nave di %s\n", players[ind]->nickname, players[index]->nickname);

send:

    for(int h=0; h<n_players; h++){
        writeString(players[h], message);
    }    

    bool elim = true;

    for(int k=0; k<SHIPS_NUM; k++){
        if(players[index]->map->ships[k].sunk == false){
            elim = false;
            break;
        }
    }

    for(int j=0; j<n_players; j++){
        if(j==index){
            if(elim){
                writeNum(players[index], 1);
                removePlayer(index);
            }
            else writeNum(players[index], 0);
        }
        else writeNum(players[j], 0);
    }

    gameInitialization();   //manda la lista aggiornata dei nicknames ai client

    return;
}
#undef BUFF_LEN

void send_maps(player_t *player, size_t index){

    char *encoded = (char *) malloc((n_players + (n_players * MAP_SIZE * MAP_SIZE) + 1) * sizeof(char));
    bzero(encoded, (n_players + (n_players * MAP_SIZE * MAP_SIZE) + 1) * sizeof(char));

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

    char *encoded = (char *) malloc(sizeof(*encoded) * (MAP_SIZE * MAP_SIZE + 1) );
    bzero(encoded, sizeof(*encoded) * (MAP_SIZE * MAP_SIZE + 1));
    
    uint8_t i;
    waitNum(player, (uint32_t *) &i);
    char *cur = encoded;

    DEBUG("[DEBUG]: %p - %p\n", cur, encoded)

    DEBUG("[DEBUG]: sending map of %s to %s\n", players[i]->nickname, player->nickname)

    for(int j=0; j<MAP_SIZE; j++){
        for(int k=0; k<MAP_SIZE; k++){
            *cur++=players[i]->map->grid[j][k];
        }
    }

    writeString(player, encoded);
    free(encoded);
    return;

}

