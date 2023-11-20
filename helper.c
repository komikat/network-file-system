#include "helper.h"
#include <arpa/inet.h>

int initserver(char *addr, char *port) {
    int status, sockfd;
    struct addrinfo hints;
    struct addrinfo *result, *p;

    memset(&hints, 0, sizeof hints); // empty struct
    hints.ai_family = AF_UNSPEC;     // ipv4/6
    hints.ai_socktype = SOCK_STREAM; // tcp
    hints.ai_flags = AI_PASSIVE;     // assigns the address of local host
                                     // to the socket

    if ((status = getaddrinfo(addr, port, &hints, &result)) != 0) {
        fprintf(stderr, "Error with getting address: %s\n",
                gai_strerror(status));
        return -1;
    }

    // result -> linked list of addrinfos for localhost:PORT
    // loop through the entire linked list
    for (p = result; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(result->ai_family, result->ai_socktype,
                             result->ai_protocol)) == -1) {
            continue;
        }

        if (bind(sockfd, result->ai_addr, result->ai_addrlen) == -1) {
            close(sockfd);
            continue;
        }

        break;
    }

    if (p == NULL) {
        fprintf(stderr, "Failed to bind\n");
        return -1;
    }

    // listen for incoming connections
    if (listen(sockfd, BACKLOG) == -1) {
        fprintf(stderr, "Error listening: %d\n", errno);
        return -1;
    }

    printf("Listening on port %s\n", port);
    return sockfd;
}

// initialises connection with server
// {address, port} -> socket fd
int initconn(char *addr, char *port) {
    int status;
    struct addrinfo hints;
    struct addrinfo *result, *p;

    memset(&hints, 0, sizeof hints); // empty struct
    hints.ai_family = AF_UNSPEC;     // ipv4/6
    hints.ai_socktype = SOCK_STREAM; // tcp
    hints.ai_flags = AI_PASSIVE;     // assigns the address of local host
                                     // to the socket

    if ((status = getaddrinfo(addr, port, &hints, &result)) != 0) {
        fprintf(stderr, "Error with getting address: %s\n",
                gai_strerror(status));
        return -1;
    }

    int sockfd; // socket descriptor
    // loop through the entire linked list
    for (p = result; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(result->ai_family, result->ai_socktype,
                             result->ai_protocol)) == -1) {
            continue;
        }

        break;
    }

    if (p == NULL) {
        fprintf(stderr, "Failed to bind\n");
        return -1;
    }

    if (connect(sockfd, result->ai_addr, result->ai_addrlen) == -1) {
        fprintf(stderr, "Error connecting to the server.\nAre you sure the "
                        "server is up?\n");
        return -1;
    } else {
        printf("Connection established!\n");
    }

    return sockfd;
}
