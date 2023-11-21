#include "./../defs.h"
#include "storage.h"

int sockfd;
int running = 0;
int jobs[MAX_JOBS_STORAGE];
// pthread_t threads[];

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
void sendTree(int jobpos) {
    char root[] = "./storage_server/root";

    struct stat statbuf;
    if (stat(root, &statbuf) == -1) {
        perror("Error getting file status");
    }
    
    char *data = (char *)malloc(sizeof(char) * 1040);
    snprintf(data, strlen(root) + 24, "REQ %d 1 %o 0 %s;", jobs[jobpos], statbuf.st_mode, root);
    int check = 1;
    while (check){  // Tries to send thrice
        check *= sendData(data, sockfd);
        if (check >= 8) {
            free(data);
            // return 3; // Error code for being unable to send
        }
    }
    free(data);

    check = sendFilesRecursively(root, sockfd, jobs[jobpos]);
    if ((check) != 0) {
        char msg[] = "Error in initializing Storage Server!\n";
        if (check != 3) sendError(sockfd, check, msg);
        else fprintf(stderr, "Error in sending data to server...\n");
    }

    char stop[32];
    sprintf(stop, "REQ %d STOP;", jobs[jobpos]);
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
    for (int i = 0; i < MAX_JOBS_STORAGE; i++) jobs[i] = -1;

    while (1) {

        // Recieve requests from Naming Server
        int r;
        if ((r = recv(sockfd, RECV_BUFFER, BUFFER,
                      0)) == -1) {
            fprintf(stderr, "Issues recv reqs: %d\n", errno);
            sendError(sockfd, 4, "Could not receive request, please resend...\n");
            continue;
        };

        // TEST
        printf("Command recieved: %s\n", RECV_BUFFER);

        // Get command string and assign job id for a free job space
        char *command = strtok(RECV_BUFFER, " ");
        int jobpos, curjob;
        for (int i = 0; i < MAX_JOBS_STORAGE; i++) {
            if (jobs[i] == -1) {
                char *jobid = strtok(NULL, " ");
                // jobid = strtok(NULL, ";");
                // printf("Job: %s\n", jobid);
                jobid[strlen(jobid) - 1] = '\0';
                sscanf(jobid, "%d", &jobs[i]);
                curjob = i;
                break;
            }
        }

        if (strcmp(command, "TREE") == 0) {
            sendTree(jobs[curjob]);
        }

        // if (strcmp(command, "REQ") == 0) {
        //     char *type = strtok(NULL, " ");
        //     if (strcmp(type, "CREATE") == 0) {
        //         //
        //     }
        //     else if (strcmp(type, "DELETE") == 0) {
        //         //
        //     }
        //     else if (strcmp(type, "READ") == 0) {
        //         //
        //     }
        //     else if (strcmp(type, "WRITE") == 0) {
        //         //
        //     }
        //     else {
        //         sendError(sockfd, 6, "Invalid request...\n");
        //         continue;
        //     }
        // }
        // else if (strcmp(command, "TREE") == 0) {
        //     sendTree(jobs[curjob]);
        // }
        // else if (strcmp(command, "TRANSFER") == 0) {
        //     //
        // }
        // else if (strcmp(command, "RECIEVE") == 0) {
        //     //
        // }
        // else {
        //     sendError(sockfd, 6, "Invalid request...\n");
        //     continue;
        // }
    }

    pthread_mutex_destroy(&serverlock);
    close(sockfd);
    return 1;
}
