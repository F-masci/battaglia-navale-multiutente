#include "helpers.h"

#define BUFF_LEN 1024
bool wait_string(player_t *player, char *buffer) {

    bzero(buffer, BUFF_LEN);
    if(read(player->socket, buffer, BUFF_LEN) <= 0) return false;
    return true;

}

bool write_string(player_t *player, char *buffer) {

    if(write(player->socket, buffer, strlen(buffer)) <= 0)  return false;
    return true;

}
#undef BUFF_LEN