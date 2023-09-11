#include "helpers.h"

/**
 * @brief Attende un numero da un giocatore specifico
 * 
 * @note La locazione di memoria dove salvare il numero deve essere già allocata
 * @note Il numero viene salvato utilizzando l'endianess dell'host
 * 
 * @param[in] _player Giocatore da cui attendere il numero
 * @param[out] _ptr Indirizzo di memoria in cui salvare il numero
 * 
 * @return Ritorna se l'azione è stata eseguita correttamente o meno
 * 
 * @retval ```true``` Numero ricevuto correttamente
 * @retval ```false``` Il numero non è stato ricevuto - viene impostato errno
 */
bool waitNum(const player_t * const _player, uint32_t * const _ptr) {
    errno = 0;
    ssize_t r;
    size_t rem = sizeof(*_ptr);
    char *cur = (char *) _ptr;
    do {
        r = read(_player->socket, cur, rem);
        if(r == (ssize_t) -1) {
            if(errno != EINTR) return false;
            else errno = 0;
        } else {
            rem -= r;
            cur += r;
        }
    } while( rem );
    *_ptr = ntohl(*_ptr);
    DEBUG("[DEBUG]: received %d from %s\n", *_ptr, _player->nickname)
    return true;
}

/**
 * @brief Invia un numero ad un giocatore specifico
 *
 * @note Il numero viene inviato utilizzando l'endianess della rete
 * 
 * @param[in] _player Giocatore a cui inviare il numero
 * @param[in] _num Numero da inviare
 * 
 * @return Ritorna se l'azione è stata eseguita correttamente o meno
 * 
 * @retval ```true``` Numero inviato correttamente
 * @retval ```false``` Il numero non è stato inviato - viene impostato errno
 */
bool writeNum(const player_t * const _player, uint32_t _num) {
    errno = 0;
    ssize_t w;
    size_t rem = sizeof(_num);
    _num = htonl(_num);
    char *cur = (char *) &_num;
    do {
        w = write(_player->socket, cur, rem);
        if(w == (ssize_t) -1) {
            if(errno != EINTR) return false;
            else errno = 0;
        } else {
            rem -= w;
            cur += w;
        }
    } while(rem);
    DEBUG("[DEBUG]: sent %d to %s\n", ntohl(_num), _player->nickname)
    return true;
}

/**
 * @brief Attende una stringa da un giocatore specifico
 *
 * @note Il buffer per la stringa verrà allocato in automatico
 * 
 * @param[in] _player Giocatore da cui attendere la stringa
 * @param[out] _buff Buffer in cui viene salvata la stringa
 * 
 * @return Ritorna se l'azione è stata eseguita correttamente o meno
 * 
 * @retval ```true``` Stringa ricevuta correttamente
 * @retval ```false``` La stringa non è stata ricevuta - viene impostato errno
 */
bool waitString(const player_t * const _player, char ** const _buff) {

    errno = 0;

    /* -- STRING LENGTH -- */
    // Prima di inviare la stringa il client invia la sua lunghezza
    
    uint32_t len;                                                       // Lunghezza della stringa che stiamo per ricevere
    if(!waitNum(_player, &len)) return false;
    DEBUG("[DEBUG]: waiting string of %d chars from %s\n", len, _player->nickname);

    /* -- STRING BUFFER -- */
    // Il buffer viene allocato alla dimensione precisa della stringa più il terminatore

    char *buffer = (char *) malloc(sizeof(*buffer) * (len+1));          // Buffer per contenere la stringa di lunghezza ```len``` più il terminatore
    if(buffer == NULL) return false;
    BZERO(buffer, len+1);
    *_buff = buffer;
    DEBUG("[DEBUG]: allocated %lu bytes\n", sizeof(*buffer) * (len+1));

    /* -- RECEIVING -- */

    ssize_t r;
    size_t rem = len;
    do {
        r = read(_player->socket, buffer, rem);
        if(r == (ssize_t) -1) {
            if(errno != EINTR) return false;
            else errno = 0;
        } else {
            rem -= r;
            buffer += r;
        }
    } while( rem );

    DEBUG("[DEBUG]: %s (%ld)\n", *_buff, strlen(*_buff))
    return true;

}

/**
 * @brief Invia una stringa ad un giocatore specifico
 * 
 * @param[in] _player Giocatore a cui inviare la stringa
 * @param[in] _buff Buffer in cui è salvata la stringa
 * 
 * @return Ritorna se l'azione è stata eseguita correttamente o meno
 * 
 * @retval ```true``` Stringa inviata correttamente
 * @retval ```false``` La stringa non è stata inviata - viene impostato errno
 */
bool writeString(const player_t * const _player, const char *_buff) {

    errno = 0;

    /* -- STRING LENGTH -- */
    // Prima di inviare la stringa il server invia la sua lunghezza

    uint32_t len = (uint32_t) strlen(_buff);                                // Lunghezza della stringa che stiamo per inviare
    if(!writeNum(_player, len)) return false;
    DEBUG("[DEBUG]: sending string of %u chars to %s\n", len, _player->nickname);
    DEBUG("[DEBUG]: %s (%ld)\n", _buff, strlen(_buff));

    /* -- SENDING -- */

    ssize_t w;
    size_t rem = len;
    do {
        w = write(_player->socket, _buff, rem);
        if(w == (ssize_t) -1) {
            if(errno != EINTR) return false;
            else errno = 0;
        } else {
            rem -= w;
            _buff += w;
        }
    } while(rem);

    return true;

}