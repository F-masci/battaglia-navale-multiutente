/**
 * @authors Cacace Elisa, Masci Francesco
 * 
 * Battaglia navale multiutente
 * 
 * Realizzazione di una versione elettronica del famoso gioco "battaglia
 * navale" con un numero di giocatori arbitrario. In questa versione piu'
 * processi client (residenti in generale su macchine diverse) sono l'interfaccia tra i
 * giocatori e il server (residente in generale su una macchina separata dai
 * client). Un client, una volta abilitato dal server, accetta come input una
 * mossa, la trasmette al server, e riceve la risposta dal server. In questa
 * versione della battaglia navale una mossa consiste oltre alle due coordinate
 * anche nell'identificativo del giocatore contro cui si vuole far fuoco.
 * Il server a sua volta quando riceve una mossa, comunica ai client se
 * qualcuno e' stato colpito se uno dei giocatori e' il vincitore (o se e' stato
 * eliminato), altrimenti abilita il prossimo client a spedire una mossa.
 * La generazione della posizione delle navi per ogni client e' lasciata alla
 * discrezione dello studente. 
 *
*/

#include "client.h"
#include "game.h"

struct sockaddr_in addr_server;
int socket_client = -1;
uint8_t num;
char **nicknames;
uint8_t me = -1;

static void exit_function(int code) {
    if(socket_client != -1) {
        if(!sendCmd(CMD_ERROR)) {
            if(errno == EPIPE) {
                exit(EXIT_FAILURE);
            } else {
                perror("Error");
                exit(EXIT_FAILURE);
            }
        }
        if(close(socket_client) == -1) exit(EXIT_FAILURE);
    }
    exit(code);
}

static void _sigint_main_handler(int sig, siginfo_t *dummy, void *dummy2) {
    PRINT("Client exit for SIGINT\n")
    exit_function(EXIT_SUCCESS);
}

static void _sigpipe_main_handler(int sig, siginfo_t *dummy, void *dummy2) {
    PRINT("Client exit for SIGPIPE\n")
    exit_function(EXIT_FAILURE);
}

int main(void) {

    errno = 0;

    /* -- SIGINT HANDLER -- */

    sigset_t set;
    sigfillset(&set);
    sigdelset(&set, SIGINT);
    if(sigprocmask(SIG_SETMASK, &set, NULL) == -1) exit(EXIT_FAILURE);
    
    struct sigaction sa;
    BZERO(&sa, sizeof(sa));
    sa.sa_sigaction = _sigint_main_handler;
    sa.sa_mask = set;
    sa.sa_flags = 0;
    sa.sa_restorer = NULL;
    if(sigaction(SIGINT, &sa, NULL) == -1) exit(EXIT_FAILURE);

    /* -- SIGPIPE HANDLER -- */

    sigemptyset(&set);
    sigaddset(&set, SIGPIPE);
    if(sigprocmask(SIG_UNBLOCK, &set, NULL) == -1) exit(EXIT_FAILURE);

    sigfillset(&set);

    BZERO(&sa, sizeof(sa));
    sa.sa_sigaction = _sigpipe_main_handler;
    sa.sa_mask = set;
    sa.sa_flags = 0;
    sa.sa_restorer = NULL;
    if(sigaction(SIGPIPE, &sa, NULL) == -1) exit(EXIT_FAILURE);

    /* -- CONFIG SERVER ADDRESS -- */

    BZERO((char*) &addr_server, sizeof(addr_server));
    addr_server.sin_family = AF_INET;
    addr_server.sin_port = htons(PORT);                             // 6500

    int udp_socket_client;
    struct sockaddr_in udp_addr_client;
    BZERO((char*) &udp_addr_client, sizeof(udp_addr_client));
    udp_addr_client.sin_family = AF_INET;
    udp_addr_client.sin_port = htons(UDP_PORT_CLN);                 // 6502
    udp_addr_client.sin_addr.s_addr = ADDRESS;                      // 0.0.0.0


    struct sockaddr_in udp_server_addr;
    BZERO((char *) &udp_server_addr, sizeof(udp_server_addr));
    udp_server_addr.sin_family = AF_INET;
    udp_server_addr.sin_port = htons(UDP_PORT_SRV);                 // 6501

    struct timeval timeout;
    timeout.tv_sec = 5;
    timeout.tv_usec = 0;

    udp_socket_client = socket(AF_INET, SOCK_DGRAM, 0);
    if(setsockopt(udp_socket_client, SOL_SOCKET, SO_REUSEADDR, &(int) {1}, sizeof(int)) == -1) EXIT_ERRNO
    if(setsockopt(udp_socket_client, SOL_SOCKET, SO_BROADCAST, &(int) {1}, sizeof(int)) == -1) EXIT_ERRNO
    if(setsockopt (udp_socket_client, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) == -1) EXIT_ERRNO
    if(bind(udp_socket_client, (struct sockaddr *) &udp_addr_client, sizeof(udp_addr_client)) == -1) EXIT_ERRNO

    socklen_t udp_server_addr_len = sizeof(udp_server_addr);

    uint8_t config;

    PRINT("Seleziona il server di gioco:\n\n")
    PRINT("\t[1] Ricerca nella rete locale\n")
    PRINT("\t[2] Inserisce ip manualmente\n")
    PRINT("\t[3] Inserisce url manualmente\n")
config_connection_loop:
    PRINT("\nComando: ")
    if(scanf("%hhu", &config) <= 0) {
        EXIT_ERRNO
        while((getchar()) != '\n');
        goto config_connection_loop;
    }
    while((getchar()) != '\n');

    switch(config) {
        case 1:

            /* -- MOSTRA INTERFACCE CLIENT NETWORK -- */

            PRINT("Seleziona rete:\n\n")

            struct ifaddrs *nics;
            while(getifaddrs(&nics) == -1) EXIT_ERRNO
            char *ip = (char*) malloc(sizeof(*ip) * 16);
            if(ip == NULL ) EXIT_ERRNO
            char *brd = (char*) malloc(sizeof(*brd) * 16);
            if(brd == NULL) EXIT_ERRNO
            struct ifaddrs *nic = nics;

            struct sockaddr_in **broadcast = (struct sockaddr_in **) malloc(sizeof(*broadcast) * 1);
            if(broadcast == NULL) EXIT_ERRNO
            size_t broadcast_len = 1, broadcast_counter = 0;

            int counter = 1;
            int ret;
            while(nic) {
                if (nic->ifa_addr->sa_family == AF_INET) {  // Interfaccia IPV4
                    while( (ret = getnameinfo(nic->ifa_addr, sizeof(struct sockaddr_in), ip, sizeof(*ip) * 16, 0, 0, NI_NUMERICHOST)) != 0 ) {
                        if(ret == EAI_SYSTEM) {
                            EXIT_ERRNO
                        } else {
                            while(fprintf(stderr, "Error: can't retrive network interface's informations\n") == -1) EXIT_ERRNO
                        }
                    }
                    while( (ret = getnameinfo(nic->ifa_broadaddr, sizeof(struct sockaddr_in), brd, sizeof(*brd) * 16, 0, 0, NI_NUMERICHOST)) != 0 ) {
                        if(ret == EAI_SYSTEM) {
                            EXIT_ERRNO
                        } else {
                            while(fprintf(stderr, "Error: can't retrive network interface's informations\n") == -1) EXIT_ERRNO
                        }
                    }
                    PRINT("\t[%d] %s -> %s\n", counter++, ip, brd);

                    if(broadcast_counter == broadcast_len) {
                        broadcast_len *= 2;
                        broadcast = reallocarray(broadcast, broadcast_len, sizeof(*broadcast));
                        if(broadcast == NULL) EXIT_ERRNO
                    }

                    broadcast[broadcast_counter++] = (struct sockaddr_in *) nic->ifa_broadaddr;

                }
                nic = nic->ifa_next;
            }

            uint8_t net;
network_loop:
            PRINT("\nRete: ")
            if(scanf("%hhu", &net) <= 0) {
                EXIT_ERRNO
                while((getchar()) != '\n');
                goto network_loop;
            }
            while((getchar()) != '\n');

            net--;
            if(net >= broadcast_counter) goto network_loop;

            udp_server_addr.sin_addr = broadcast[net]->sin_addr;                          // 255.255.255.255

            freeifaddrs(nics);
            nics = NULL;
            free(ip);
            ip = NULL;
            free(brd);
            brd = NULL;
            free(broadcast);
            broadcast = NULL;
            break;

        case 2: 
            
            PRINT("IP: ")

            char *addr = NULL;

            while(scanf("%ms", &addr) <= 0) {
                EXIT_ERRNO
                while((getchar()) != '\n');
            }
            while((getchar()) != '\n');
            if( !inet_aton(addr, &udp_server_addr.sin_addr) ) goto config_connection_loop;

            free(addr);
            addr = NULL;
            break;

        case 3:

            PRINT("URL: ")

            char *url;
            struct hostent *hp;
            
            while(scanf("%ms", &url) <= 0) {
                EXIT_ERRNO
                while((getchar()) != '\n');
            }
            while((getchar()) != '\n');
            hp = gethostbyname(url);
            if(hp == NULL) goto config_connection_loop;
            memcpy(&udp_server_addr.sin_addr, hp->h_addr, hp->h_length);
            free(url);
            break;

        default: goto config_connection_loop;
    }
    
    while(sendto(udp_socket_client, NULL, 0, 0, (struct sockaddr *) &udp_server_addr, sizeof(udp_server_addr)) == -1) EXIT_ERRNO
    if(recvfrom(udp_socket_client, NULL, 0, MSG_TRUNC, (struct sockaddr *) &udp_server_addr, &udp_server_addr_len) == -1){
        if(errno != EAGAIN) EXIT_ERRNO
        PRINT("Nessun server trovato\n")
        goto config_connection_loop;
    }

    udp_server_addr.sin_port = ntohs(udp_server_addr.sin_port);
    DEBUG("[DEBUG]: received answer from %s (port %d)\n", inet_ntoa(udp_server_addr.sin_addr), udp_server_addr.sin_port)
    if(udp_server_addr.sin_port != 6501) {
        PRINT("Server non valido\n")
        goto config_connection_loop;
    }

    while(close(udp_socket_client) == -1) EXIT_ERRNO
    addr_server.sin_addr.s_addr = udp_server_addr.sin_addr.s_addr;

    clientConnection();                     // CONNESSIONE CLIENT
    mapInitialization();                    // INIZIALIZZAZIONE MAPPA
    gameInitialization();                   // INIZIALIZZAZIONI DATI DI GIOCO

    cmd_t cmd;
    char *message = NULL;
    uint16_t alive;

wait_turn:

    PRINT("In attesa del proprio turno\n")

    cmd = waitCmd();

    switch(cmd) {

        case CMD_STATUS:

            if(!waitString(&message)) EXIT_ERRNO
            PRINT("\n\t%s\n", message)
            free(message);
            message = NULL;

            if(!waitString(&message)) EXIT_ERRNO
            PRINT("\t%s\n", message);
            free(message);
            message = NULL;

            if(!waitNum((uint32_t *) &alive)) EXIT_ERRNO
            if((uint8_t) (alive-2) == num) {
                goto wait_turn;
            } else if((uint8_t) (alive-1) == num) {
                PRINT("Sei stato eliminato\n")
                return EXIT_SUCCESS;
            } else {
                PRINT("%s e' stato eliminato\n", nicknames[alive])

                for(uint8_t i=alive; i<num-1; i++){
                    strcpy(nicknames[i], nicknames[i+1]);
                }
                free(nicknames[num-1]);
                nicknames[num-1] = NULL;
                num--;
                if(num == 1){
                    PRINT("Hai vinto!\n")
                    return EXIT_SUCCESS;
                }
                goto wait_turn;
            }
            goto wait_turn;
        
        case CMD_TURN:

            clrscr();
            PRINT("È il tuo turno\n")

main_loop:

            PRINT("\nSeleziona un comando:\n\n")
            PRINT("\t[1] Visualizza mappe giocatori\n")
            PRINT("\t[2] Visualizza una mappa\n")
            PRINT("\t[3] Invia comando\n\n")

            PRINT("Comando: ")
            if(scanf("%hhu", &cmd) <= 0) {
                EXIT_ERRNO
                while((getchar()) != '\n');
                goto main_loop;
            }
            while((getchar()) != '\n');

            switch(cmd) {
                case 1: 
                    if(!sendCmd(CMD_GET_MAPS)) EXIT_ERRNO
                    printMaps();
                    goto main_loop;

                case 2:
                    if(!sendCmd(CMD_GET_MAP)) EXIT_ERRNO

                    char *encoded = NULL;
                    uint8_t p = choosePlayer(true);
                    if(!writeNum((uint32_t) p)) EXIT_ERRNO
                    if(!waitString(&encoded)) EXIT_ERRNO

                    if(p == me) printMap(encoded, true);
                    else printMap(encoded, false);

                    free(encoded);
                    encoded = NULL;

                    goto main_loop;

                case 3: 
                    if(!sendCmd(CMD_MOVE)) EXIT_ERRNO
                    makeMove();
                    goto wait_turn;

                default: goto main_loop;
            }
            goto wait_turn;
        
        case CMD_CLOSE_CONNECTION: return EXIT_SUCCESS;

        case CMD_ERROR: EXIT_ERRNO;

        default: goto wait_turn;
    }

    return EXIT_SUCCESS;

}