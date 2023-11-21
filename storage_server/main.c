#include "./../defs.h"
#include "storage.h"

int sockfd;
int running = 0;
int jobs[MAX_JOBS_STORAGE];

char RECV_BUFFER[BUFFER];

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

// Function to initialize and send file tree
void sendTree() {
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

    char stop[] = "STOP;";
    sendData(stop, sockfd);
}

int main() {

    // Initiate connection with Naming Server
    sockfd = initconn("localhost", PORT_NSS);
    if (sockfd != -1) running = 1;
    else {
        printf("Could not initialize server...\n");
        return 0;
    }

    // Initialize serverlock and heartbeat
    pthread_mutex_init(&serverlock, NULL);
    // pthread_t heart;
    // pthread_create(&heart, NULL, heartbeat, NULl);
    // pthread_join(heart, NULL);

    // Inititlize job array
    for (int i = 0; i < MAX_JOBS_STORAGE; i++) jobs[i] = NULL;

    while (1) {

        // Recieve requests from Naming Server
        int r;
        if ((r = recv(sockfd, RECV_BUFFER, BUFFER,
                      0)) == -1) {
            fprintf(stderr, "Issues recv reqs: %d\n", errno);
            sendError(sockfd, 4, "Could not receive request, please resend...\n");
            continue;
        };

        // Get command string and assign job id for a free job space
        char *command = strtok(RECV_BUFFER, " ");
        int jobpos;
        for (int i = 0; i < MAX_JOBS_STORAGE; i++) {
            if (jobs[i] == NULL) {
                
            }
        }

        if (strcmp(command, "REQ")) {
            //
        }
        else if (strcmp(command, "TREE")) {

            // get jobid and assign a free job
            char *jobid = strtok(NULL, " ");
            sscanf(jobid, "%d", );
        }
        else if (strcmp(command, "TRANSFER")) {
            //
        }
        else if (strcmp(command, "RECIEVE")) {
            //
        }
    }

    pthread_mutex_destroy(&serverlock);
    close(sockfd);
    return 1;
}
