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
#include <dirent.h>
#include <sys/stat.h>

#define PORT_NSS "2049" // ns <-> ss
#define PORT_NSC "2050" // client <-> ns
#define PORT_SSC "2051" // client <-> ss

#define BACKLOG 10
#define BUFFER 1024
#define MAX_FILES_PER_DIRECTORY 256

#define HEARTBEAT 1

// Naming Serever States

#define INITIALIZING 0
#define CONNECTING_TO_STORAGE 1
#define READY 2
