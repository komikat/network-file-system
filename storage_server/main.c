#include "./../defs.h"
#include "storage.h"

int sockfd;
int running = 0;

pthread_mutex_t serverlock;


// Thred for heartbeat
void *heartbeat() {
    char beat[] = "heartbeat";
    while (running) {
        if ((send(sockfd, beat, strlen(beat), 0)) == -1) {
            fprintf(stderr, "Error sending Heartbeat\n", errno);
        }
        sleep(HEARTBEAT);
    }
}

int main() {
    sockfd = initconn("localhost", PORT_NSS);
    if (sockfd != -1) running = 1;
    else {
        printf("Could not initialize server...\n");
        return 0;
    }

    pthread_mutex_init(&serverlock, NULL);
    // pthread_t heart;
    // pthread_create(&heart, NULL, heartbeat, NULl);
    // pthread_join(heart, NULL);

    char root[] = "./storage_server/root";

    struct stat statbuf;
    if (stat(root, &statbuf) == -1) {
        perror("Error getting file status");
    }

    
    char *data = (char *)malloc(sizeof(char) * 1040);
    snprintf(data, strlen(root) + 24, "1 %o 0 %s;", statbuf.st_mode, root);
    int check = 1;
    while (check){  // Tries to send thrice
        check *= sendData(data, sockfd);
        if (check >= 8) {
            free(data);
            // return 3; // Error code for being unable to send
        }
    }
    free(data);

    check = sendFilesRecursively(root, sockfd);
    if ((check) != 0) {
        char msg[] = "Error in initializing Storage Server!\n";
        if (check != 3) sendError(sockfd, check, msg);
        else fprintf(stderr, "Error in sending data to server...\n");
    }

    char stop[] = "STOP";
    if ((send(sockfd, stop, strlen(stop), 0)) == -1) {
        fprintf(stderr, "Issues sending message: %d\n", errno);
        return 2;
    }

    pthread_mutex_destroy(&serverlock);
    close(sockfd);
    return 1;
}
