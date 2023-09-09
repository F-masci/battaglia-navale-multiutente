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
int socket_client;
cell_t **map;
uint8_t num;

#define BUFF_LEN 1024
int main(void) {

    int i;

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
    setsockopt(udp_socket_client, SOL_SOCKET, SO_REUSEADDR, &(int) {1}, sizeof(int));
    setsockopt(udp_socket_client, SOL_SOCKET, SO_BROADCAST, &(int) {1}, sizeof(int));
    setsockopt (udp_socket_client, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));
    bind(udp_socket_client, (struct sockaddr *) &udp_addr_client, sizeof(udp_addr_client));

    socklen_t udp_server_addr_len = sizeof(udp_server_addr);

    uint8_t config;

    PRINT("Seleziona il server di gioco:\n\n")
    PRINT("\t[1] Ricerca nella rete locale\n")
    PRINT("\t[2] Inserisce ip manualmente\n")
    PRINT("\t[3] Inserisce url manualmente\n")
config_connection_loop:
    PRINT("\nComando: ")
    if(scanf("%hhu", &config) <= 0) {
        while((getchar()) != '\n');
        goto config_connection_loop;
    }

    switch(config) {
        case 1:

            /* -- SHOW CLIENT NETWORK INTERFACES-- */

            PRINT("Seleziona rete:\n\n")

            struct ifaddrs *nics;
            getifaddrs(&nics);
            char *ip = (char*) malloc(sizeof(*ip) * 16);
            char *brd = (char*) malloc(sizeof(*brd) * 16);
            struct ifaddrs *nic = nics;

            struct sockaddr_in **broadcast = (struct sockaddr_in **) malloc(sizeof(*broadcast) * 1);
            size_t broadcast_len = 1, broadcast_counter = 0;

            int counter = 1;
            while(nic) {
                if (nic->ifa_addr->sa_family == AF_INET) {  // Interfaccia IPV4
                    getnameinfo(nic->ifa_addr, sizeof(struct sockaddr_in), ip, sizeof(*ip) * 16, 0, 0, NI_NUMERICHOST);
                    getnameinfo(nic->ifa_broadaddr, sizeof(struct sockaddr_in), brd, sizeof(*brd) * 16, 0, 0, NI_NUMERICHOST);
                    printf("\t[%d] %s -> %s\n", counter++, ip, brd);

                    if(broadcast_counter == broadcast_len) {
                        broadcast_len *= 2;
                        broadcast = reallocarray(broadcast, broadcast_len, sizeof(*broadcast));
                    }

                    broadcast[broadcast_counter++] = (struct sockaddr_in *) nic->ifa_broadaddr;

                }
                nic = nic->ifa_next;
            }

            uint8_t net;
network_loop:
            PRINT("\nRete: ")
            if(scanf("%hhu", &net) <= 0) {
                while((getchar()) != '\n');
                goto network_loop;
            }

            net--;
            if(net >= broadcast_counter) goto network_loop;

            udp_server_addr.sin_addr = broadcast[net]->sin_addr;                          // 255.255.255.255

            freeifaddrs(nics);
            free(ip);
            free(brd);
            free(broadcast);
            break;

        case 2: 
            
            ;
            char *addr;
            PRINT("IP: ")
            scanf("%ms", &addr);
            if(inet_aton(addr, &udp_server_addr.sin_addr) == 0) goto config_connection_loop;
            free(addr);
            break;

        case 3:

            ;
            char *url;
            struct hostent *hp;
            PRINT("URL: ")
            scanf("%ms", &url);
            hp = gethostbyname(url);
            if(hp == NULL) goto config_connection_loop;
            bcopy(hp->h_addr, &udp_server_addr.sin_addr, hp->h_length);
            free(url);
            break;

        default: goto config_connection_loop;
    }
    
    sendto(udp_socket_client, NULL, 0, 0, (struct sockaddr *) &udp_server_addr, sizeof(udp_server_addr));
    if(recvfrom(udp_socket_client, NULL, 0, MSG_TRUNC, (struct sockaddr *) &udp_server_addr, &udp_server_addr_len) == -1){
        PRINT("Nessun server trovato\n")
        goto config_connection_loop;
    }

    udp_server_addr.sin_port = ntohs(udp_server_addr.sin_port);
    DEBUG("[DEBUG]: received answer from %s (port %d)\n", inet_ntoa(udp_server_addr.sin_addr), udp_server_addr.sin_port)
    if(udp_server_addr.sin_port != 6501) {
        PRINT("Server non valido\n")
        goto config_connection_loop;
    }

    close(udp_socket_client);
    addr_server.sin_addr.s_addr = udp_server_addr.sin_addr.s_addr;

    clientConnection();                     // CLIENT CONNECTION

    map = (cell_t **) malloc(MAP_SIZE * sizeof(*map));
    for(i=0; i<MAP_SIZE; i++) map[i] = (cell_t *) malloc(MAP_SIZE * sizeof(*(map[i])));

    mapInitialization();                   // MAP INITIALIZATION
    
    gameInitialization(0);                  // INITIALIZATION OF DATA NEEDED FOR THE GAME

    cmd_t cmd;
    char *buffer = (char *)malloc(sizeof(char) * BUFF_LEN);
    char *message = (char *)malloc(sizeof(char) * BUFF_LEN);
    uint32_t alive=0;

wait_turn:

    PRINT("In attesa del proprio turno\n")

    //prima di ogni turno il client riceve comunicazioni dal server

    cmd = waitCmd();
    if(cmd == CMD_STATUS) {
        waitString(&message);
        printf("%s\n", message);
        BZERO(message, BUFF_LEN);
        waitString(&message);
        printf("%s\n", message);
        waitNum(&alive);
        if(alive == 1){
            PRINT("Hai perso\n");
            return EXIT_SUCCESS;
        }
        else if(alive == 0){
            gameInitialization(1);      //update nicknames
            if(num == 1){
                PRINT("Hai vinto!");
                return EXIT_SUCCESS;
            }
            goto wait_turn;
        }
    }
    else if(cmd == CMD_TURN){

        clrscr();
        PRINT("È il tuo turno\n")

    main_loop:

        //clrscr();

        PRINT("\nSeleziona un comando:\n\n")
        PRINT("\t[1] Visualizza mappe giocatori\n")
        PRINT("\t[2] Visualizza una mappa\n")
        PRINT("\t[3] Invia comando\n\n")

        PRINT("Comando: ")
        if(scanf("%hhu", &cmd) <= 0) {
            while((getchar()) != '\n');
            goto main_loop;
        }

        BZERO(buffer, BUFF_LEN);
        switch(cmd) {
            case 1: 
                sendCmd(CMD_GET_MAPS);
                print_maps();
                goto main_loop;

            case 2:
                sendCmd(CMD_GET_MAP);
                print_map();
                goto main_loop;

            case 3: 
                sendCmd(CMD_MOVE);
                make_move();
                goto wait_turn;

            default: goto main_loop;
        }
    }
    else goto wait_turn;

    return EXIT_SUCCESS;

}
#undef BUFF_LEN