#include "./../defs.h"

#define BACKLOG 10
char CLIENT_BUFFER[CLIENT_BUFFER_LENGTH];
int expect_storage;

// get characters into the client buffer with client buffer length
// remove trailing \n
void fgeter() {
    fgets(CLIENT_BUFFER, CLIENT_BUFFER_LENGTH, stdin);
    CLIENT_BUFFER[strcspn(CLIENT_BUFFER, "\n")] = 0;
}

void sender(int sockfd, char *buf, int len) {
    if ((send(sockfd, buf, len, 0)) == -1) {
        fprintf(stderr, "Issues sending req: %d\n", errno);
        exit(0);
    };
}

// gracefully disconnect
void gracedc(int sockfd) {
    strcpy(CLIENT_BUFFER, "END");
    sender(sockfd, CLIENT_BUFFER, CLIENT_BUFFER_LENGTH);
}

int main() {
    int sockfd = initconn("localhost", PORT_NSC);
    strcpy(CLIENT_BUFFER, "ACK");
    sender(sockfd, CLIENT_BUFFER, CLIENT_BUFFER_LENGTH);
    recver(sockfd, CLIENT_BUFFER, CLIENT_BUFFER_LENGTH, 0);

    if (strcmp(CLIENT_BUFFER, "ACK") == 0) {
        while (1) {
            printf("%s", "Message: ");
            fgeter();
            if (!strcmp(CLIENT_BUFFER, "exit")) {
                printf("Exiting...");
                gracedc(sockfd);
                exit(0);
            }
            printf("%s\n", CLIENT_BUFFER);
            char *msg = strdup(CLIENT_BUFFER);
            char *msg_real = strdup(msg);
            char *saveptr = strdup(msg);


            char *token = strtok_r(msg, " ", &saveptr);
            if (!strcmp(token, "CREATE") || !strcmp(token, "DELETE") ||
                !strcmp(token, "GETINFO") || !strcmp(token, "COPY") || !strcmp(token, "LIST")) {
                expect_storage = 0;
                printf("no storage\n");
            } else if (!strcmp(token, "READ") || !strcmp(token, "WRITE")) {
                printf(" storage\n");
                expect_storage = 1;
            } else {
                printf("ERROR: UNRECOGNIZED COMMAND. RE-Enter\n");
                continue;
            }

            // CREATE
            // - send file name
            // - get list of storage servers with ips
            // - select and send to the storage server using the ip
            // - storage server creates a file and sends back the update to the naming server

            // DELETE
            // same

            // COPY
            // - send path1 path2
            // - recv ip1 ip2
            // - ns sends copy commands to both storage servers

            // READ
            // - send relative filename
            // - get back a list of filepath and storage server ip
            // - get to select from list
            // - send read req to the respective storage server

            // WRITE
            // - same

            // GETINGO
            // - send list of options of files
            // - simple

            // LIST
            // - simple


            sender(sockfd, msg_real, strlen(msg_real));
            recver(sockfd, CLIENT_BUFFER, CLIENT_BUFFER_LENGTH, 0);
            if (!expect_storage) { // no need to contact storage server
                printf("%s\n", CLIENT_BUFFER);
                if (strchr(CLIENT_BUFFER, '\n')) {
                    char option[2] = "\0\0";
                    scanf("%s", option);
                    sender(sockfd, option, strlen(option));
                    recver(sockfd, CLIENT_BUFFER, CLIENT_BUFFER_LENGTH, 0);
                    printf("%s\n", CLIENT_BUFFER);
                }
            } else { // contacting storage server
                printf("%s\n", CLIENT_BUFFER);
                if (strchr(CLIENT_BUFFER, '\n')) {
                    char option[2] = "\0\0";
                    scanf("%s", option);
                    sender(sockfd, option, strlen(option));
                    recver(sockfd, CLIENT_BUFFER, CLIENT_BUFFER_LENGTH, 0);
                    printf("%s\n", CLIENT_BUFFER);
                }
                // now to send to storage server
                // TODO : read needs to send file size as well
                int storagesock = initconn("CLIENT_BUFFER", PORT_SSC);
                sender(storagesock, msg_real, strlen(msg_real));
                recver(storagesock, CLIENT_BUFFER, CLIENT_BUFFER_LENGTH, 0);
                close(storagesock);
            }
            printf("OPERATION COMPLETE, ON TO THE NEXT!\n");
        }
    } else {
        printf("Wrong ACKKNOWLEDGEMENT recvd.\nAborting.\n");
        gracedc(sockfd);
    }

    close(sockfd);
}
