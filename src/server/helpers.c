#include "helpers.h"

// Il puntatore deve essere allocato
bool waitNum(player_t *player, uint32_t *ptr) {

    if(recv(player->socket, ptr, sizeof(*ptr), MSG_WAITALL) < (ssize_t) sizeof(*ptr)) return false;
    *ptr = ntohl(*ptr);
    DEBUG("[DEBUG]: Received %d\n", *ptr)
    return true;

}

bool writeNum(player_t *player, uint32_t num) {

    num = htonl(num);
    if(write(player->socket, &num, sizeof(num)) < (ssize_t) sizeof(num)) return false;
    return true;

}

// Il puntatore verrà allocato in automatico
bool waitString(player_t *player, char **ptr) {

    // Prima di ogni stringa il client invia la sua lunghezza
    uint32_t len;
    if(!waitNum(player, &len)) return false;

    PRINT("[SERVER]: waiting string of %d chars\n", len);

    char *buffer = (char *) malloc(sizeof(*buffer) * (len+1));
    bzero(buffer, len+1);   // Il carattere in più è per il terminatore
    if(recv(player->socket, buffer, len, MSG_WAITALL) < (ssize_t) len) return false;
    PRINT("[SERVER]: %s (%ld)\n", buffer, strlen(buffer))
    *ptr = buffer;
    return true;

}

bool writeString(player_t *player, char *buffer) {

    uint32_t len = (uint32_t) strlen(buffer);
    writeNum(player, len);

    if(write(player->socket, buffer, len) < (ssize_t) len)  return false;
    return true;

}