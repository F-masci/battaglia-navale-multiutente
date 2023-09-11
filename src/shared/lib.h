#pragma once

#define _GNU_SOURCE
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <ifaddrs.h>
#include <pthread.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <arpa/inet.h>
#include <stdarg.h>

#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <string.h>
#include <stdbool.h>
#include <errno.h>
#include <ctype.h>
#include <math.h>

#define PRINT(...)  { printf(__VA_ARGS__); fflush(stdout); }

#ifdef PROD
#define DEBUG(...) ;
#else
#define DEBUG(...) { printf(__VA_ARGS__); fflush(stdout); }
#endif

#define clrscr() PRINT("\033[1;1H\033[2J")

#define CHAR_FOR_BYTES(bytes) ( (uint8_t) ceil(log10(pow(2., 8 * (double) bytes))) )

#define BZERO(ptr, s) memset(ptr, 0, s)

#define EXIT_ERRNO { if(errno != 0 && errno != EINTR) { perror("Error"); raise(SIGINT); } else { errno = 0; } }
#define CHECK_ERRNO(err) if(errno != 0) { perror("Error"); errno = 0; } else { fprintf(stderr, err); }