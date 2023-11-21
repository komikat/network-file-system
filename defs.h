#ifndef _DEFS_H_
#define _DEFS_H_

#include "helper.h"
#include <arpa/inet.h>
#include <dirent.h>
#include <errno.h>
#include <netdb.h>
#include <poll.h>
#include <pthread.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>

#define PORT_NSS "8001" // ns <-> ss
#define PORT_NSC "8002" // client <-> ns
#define PORT_SSC "8003" // client <-> ss

#define BACKLOG 10
#define BUFFER 1024
#define MAX_FILES_PER_DIRECTORY 256
#define MAX_CLIENT 64 // max number of clients and servers

#define HEARTBEAT 1

// Naming Serever States

#define INITIALIZING 0
#define CONNECTING_TO_STORAGE 1
#define READY 2

// char ROOT_ADDRESS[] = "./storage_server/root"
// client stuff
#define CLIENT_BUFFER_LENGTH 1024

#endif
