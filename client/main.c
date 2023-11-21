#include "./../defs.h"

#define BACKLOG 10
char CLIENT_BUFFER[CLIENT_BUFFER_LENGTH];

// get characters into the client buffer with client buffer length
// remove trailing \n
void fgeter() {
    fgets(CLIENT_BUFFER, CLIENT_BUFFER_LENGTH, stdin);
    CLIENT_BUFFER[strcspn(CLIENT_BUFFER, "\n")] = 0;
}

int sender(int sockfd) {
    if ((send(sockfd, CLIENT_BUFFER, CLIENT_BUFFER_LENGTH, 0)) == -1) {
        fprintf(stderr, "Issues sending req: %d\n", errno);
        return 0;
    };
    return 1;
}

int main() {
    int sockfd = initconn("localhost", PORT_NSC);
    int check = 0;

    while(1) {
        printf("%s", "==> ");
        fgeter();

        if (strcmp(CLIENT_BUFFER, "EXIT") == 0) break;

        if ((check = sender(sockfd))) printf("Command Sent: %s\n", CLIENT_BUFFER);

        if (cehck) recver(sockfd, CLIENT_BUFFER, CLIENT_BUFFER_LENGTH, 0);
    }

    close(sockfd);
}
