#pragma once

extern int socket_server;
extern struct sockaddr_in addr_server;

extern void *client_thread(void *args);