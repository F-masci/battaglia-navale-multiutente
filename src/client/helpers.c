#include "helpers.h"

extern int socket_client;

// Il puntatore deve essere allocato
bool waitNum(uint32_t *ptr) {

    if(recv(socket_client, ptr, sizeof(*ptr), MSG_WAITALL) < (ssize_t) sizeof(*ptr)) return false;
    *ptr = ntohl(*ptr);
    DEBUG("[DEBUG]: Received %d\n", *ptr)
    return true;

}

bool writeNum(uint32_t num) {

    num = htonl(num);
    if(write(socket_client, &num, sizeof(num)) < (ssize_t) sizeof(num)) return false;
    return true;

}

// Il puntatore verrà allocato in automatico
bool waitString(char **ptr) {

    // Prima di ogni stringa il server invia la sua lunghezza
    uint32_t len;
    if(!waitNum(&len)) return false;

    DEBUG("[DEBUG]: Waiting string of %d chars\n", len);

    char *buffer = (char *) malloc(sizeof(*buffer) * (len+1));
    bzero(buffer, len+1);   // Il carattere in più è per il terminatore
    if(recv(socket_client, buffer, len, MSG_WAITALL) < (ssize_t) len) return false;
    DEBUG("[DEBUG]: %s (%ld)\n", buffer, strlen(buffer))
    *ptr = buffer;
    return true;

}

bool writeString(char *buffer) {

    uint32_t len = (uint32_t) strlen(buffer);
    writeNum(len);

    if(write(socket_client, buffer, len) < (ssize_t) len)  return false;
    return true;

}