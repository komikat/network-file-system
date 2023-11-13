#include "helper.h"
#include <arpa/inet.h>
#include <errno.h>
#include <netdb.h>
#include <pthread.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <time.h>
#include <unistd.h>

#define PORT_NSS "2049" // ns <-> ss
#define PORT_NSC "2050" // client <-> ns
#define PORT_SSC "2051" // client <-> ss
#define BACKLOG 10
