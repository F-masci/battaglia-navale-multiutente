#include "game.h"

extern uint8_t n_players;
extern player_t **players;

void gameInitialization(void){

    size_t nick_len;
    char *encoded = (char *) malloc((n_players * NICKNAME_LEN + 1) * sizeof(*encoded));
    bzero(encoded, (n_players * NICKNAME_LEN + 1) * sizeof(*encoded));

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
    
}

#define BUFF_LEN 1024
void get_move(player_t *player){

    uint8_t index, x1, y1;
    int i;
    char *message = (char *) malloc(BUFF_LEN * sizeof(*message));
    bzero(message, BUFF_LEN * sizeof(*message));

    char *encoded_move = NULL;

    waitString(player, &encoded_move);

    index = encoded_move[0] - '0';
    x1 = encoded_move[1] - '0';
    y1 = encoded_move[2] - '0';

    if(players[index]->map->grid[y1][x1] == '1'){
        players[index]->map->grid[y1][x1] = '2';
        sprintf(message, "%s ha colpito %s [%hhu; %hhu]\n", player->nickname, players[index]->nickname, x1, y1);
    }
    else if(players[index]->map->grid[y1][x1] == '0' || players[index]->map->grid[y1][x1] == '3'){
        players[index]->map->grid[y1][x1] = '3';
        sprintf(message, "%s ha mancato %s [%hhu; %hhu]\n", player->nickname, players[index]->nickname, x1, y1);
    }
    else if(players[index]->map->grid[y1][x1] == '2'){
        sprintf(message, "%s ha colpito una parte di una nave di %s già colpita [%hhu; %hhu]\n", player->nickname, players[index]->nickname, x1, y1);
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
                sprintf(message, "%s ha affondato DESTROYER di %s\n", player->nickname, players[index]->nickname);
                break;
            case 3:
                sprintf(message, "%s ha affondato SUBMARINE di %s\n", player->nickname, players[index]->nickname);
                break;
            case 4:
                sprintf(message, "%s ha affondato BATTLESHIP di %s\n", player->nickname, players[index]->nickname);
                break;
            case 5:
                sprintf(message, "%s ha affondato CARRIER di %s\n", player->nickname, players[index]->nickname);
                break;
            default: break;
        }

        goto send;

        next:
        ;
    }

    sprintf(message, "%s non ha affondato nessuna nave di %s\n", player->nickname, players[index]->nickname);

send:

    for(uint8_t h=0; h<n_players; h++){
        writeString(players[h], message);
    }    

    bool elim = true;

    for(int k=0; k<SHIPS_NUM; k++){
        if(players[index]->map->ships[k].sunk == false){
            elim = false;
            break;
        }
    }

    for(uint8_t j=0; j<n_players; j++){
        if(j == index) {
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


void send_maps(player_t *player){
    
    uint8_t index = player->index;
    
    // N_PLAYERS * MAP_SIZE * MAP_SIZE => mappa di ogni giocatore
    // +
    // N_PLAYERS => prefisso di ogni mappa per capire se è il giocatore o è q
    // +
    // 1 => terminatore di stringa
    char *encoded = (char *) malloc(((n_players * MAP_SIZE * MAP_SIZE) + n_players + 1) * sizeof(*encoded));
    bzero(encoded, ((n_players * MAP_SIZE * MAP_SIZE) + n_players + 1) * sizeof(*encoded));

    DEBUG("[DEBUG]: allocated %ld bytes\n", ((n_players * MAP_SIZE * MAP_SIZE) + n_players + 1) * sizeof(*encoded))

    for(uint8_t i=0; i<n_players; i++){
        if(i != index){
            *encoded++ = '0' + (i+1);
        } else {
            *encoded++ = 'M';
        }

        for(uint8_t j=0; j<MAP_SIZE; j++){
            memcpy(encoded, players[i]->map->grid[j], MAP_SIZE * sizeof(*encoded));
            encoded += MAP_SIZE;
        }
    }

    encoded -= (n_players * MAP_SIZE * MAP_SIZE) + n_players;

    DEBUG("[DEBUG]: sending %s (%ld chars)\n", encoded, strlen(encoded))

    writeString(player, encoded);
    free(encoded);

}

void send_map(player_t *player){

    // MAP_SIZE * MAP_SIZE => mappa del giocatore
    // +
    // 1 => terminatore di stringa
    char *encoded = (char *) malloc((MAP_SIZE * MAP_SIZE + 1) * sizeof(*encoded));
    bzero(encoded, (MAP_SIZE * MAP_SIZE + 1) * sizeof(*encoded));

    DEBUG("[DEBUG]: allocated %ld bytes\n", (MAP_SIZE * MAP_SIZE + 1) * sizeof(*encoded))
    
    uint8_t i;
    waitNum(player, (uint32_t *) &i);

    DEBUG("[DEBUG]: sending map of %s to %s\n", players[i]->nickname, player->nickname)

    for(int j=0; j<MAP_SIZE; j++){
        for(int k=0; k<MAP_SIZE; k++){
            *encoded++ = players[i]->map->grid[j][k];
        }
    }
    encoded -= MAP_SIZE * MAP_SIZE;

    DEBUG("[DEBUG]: sending %s (%ld chars)\n", encoded, strlen(encoded))

    writeString(player, encoded);
    free(encoded);

}

