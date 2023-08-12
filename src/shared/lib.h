#pragma once

#define _GNU_SOURCE
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <pthread.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>

#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <string.h>
#include <stdbool.h>
#include <errno.h>
#include <ctype.h>

#define PRINT(...)  { printf(__VA_ARGS__); fflush(stdout); }

#define clrscr() PRINT("\033[1;1H\033[2J")