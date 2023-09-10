#include "cmd.h"

/**
 * @brief Attende un comando da un giocatore specifico
 * 
 * @param[in] _player Giocatore da cui attendere il comando
 * 
 * @return Ritorna il comando ricevuto
 * 
 * @retval ```CMD_ERROR``` in caso di errore - viene impostato errno
 */
cmd_t waitCmd(const player_t * const _player) {
    
    cmd_t cmd;
    while( read(_player->socket, &cmd, sizeof(cmd)) == (ssize_t) -1){
        if(errno != EINTR) return CMD_ERROR;
        else errno = 0;
    }
    return cmd;

}

/**
 * @brief Invia un comando ad un giocatore specifico
 *
 * @param[in] _player Giocatore a cui inviare il comando
 * @param[in] _cmd Comando da inviare
 * 
 * @return Ritorna se l'azione è stata eseguita correttamente o meno
 * 
 * @retval ```true``` Comando inviato correttamente
 * @retval ```false``` Il comando non è stato inviato - viene impostato errno
 */
bool sendCmd(const player_t * const _player, const cmd_t _cmd) {

    while( write(_player->socket, &_cmd, sizeof(_cmd)) == (ssize_t) -1) {
        if(errno != EINTR) return CMD_ERROR;
        else errno = 0;
    }
    return true;

}