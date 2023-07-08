#pragma once

#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <pthread.h>

#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <string.h>

#define PRINT(...) {         \
    printf(__VA_ARGS__);     \
    fflush(stdout);          \
}
