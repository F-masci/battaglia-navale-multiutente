#include "cmd.h"

extern int socket_client;

/**
 * @brief Attende un comando dal server
 * 
 * @return Ritorna il comando ricevuto
 * 
 * @retval ```CMD_ERROR``` in caso di errore - viene impostato errno
 */
cmd_t waitCmd(void) {
    
    errno = 0;
    cmd_t cmd;
    while( read(socket_client, &cmd, sizeof(cmd)) == (ssize_t) -1){
        if(errno != EINTR) return CMD_ERROR;
        else errno = 0;
    }
    return cmd;

}

/**
 * @brief Invia un comando al server
 *
 * @param[in] _cmd Comando da inviare
 * 
 * @return Ritorna se l'azione è stata eseguita correttamente o meno
 * 
 * @retval ```true``` Comando inviato correttamente
 * @retval ```false``` Il comando non è stato inviato - viene impostato errno
 */
bool sendCmd(const cmd_t _cmd) {

    errno = 0;
    while( write(socket_client, &_cmd, sizeof(_cmd)) == (ssize_t) -1) {
        if(errno != EINTR) return CMD_ERROR;
        else errno = 0;
    }
    return true;

}