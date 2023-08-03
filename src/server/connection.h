#pragma once

#include "../lib/lib.h"
#include "../config/config.h"

extern int socket_server;
extern struct sockaddr_in addr_server;

extern void *client_handler(void *args);