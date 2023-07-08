#pragma once

int socket_server;
struct sockaddr_in addr_server;

extern void waitConnections(void);