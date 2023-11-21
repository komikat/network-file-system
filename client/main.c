#include "./../defs.h"

#define BACKLOG 10
char CLIENT_BUFFER[CLIENT_BUFFER_LENGTH];

// get characters into the client buffer with client buffer length
// remove trailing \n
void fgeter() {
    fgets(CLIENT_BUFFER, CLIENT_BUFFER_LENGTH, stdin);
    CLIENT_BUFFER[strcspn(CLIENT_BUFFER, "\n")] = 0;
}

void sender(int sockfd) {
    if ((send(sockfd, CLIENT_BUFFER, CLIENT_BUFFER_LENGTH, 0)) == -1) {
        fprintf(stderr, "Issues sending req: %d\n", errno);
        exit(0);
    };
}

int main() {
    int sockfd = initconn("localhost", PORT_NSC);

    printf("%s", "Message: ");
    fgeter();
    printf("%s\n", CLIENT_BUFFER);
    sender(sockfd);

    recver(sockfd, CLIENT_BUFFER, CLIENT_BUFFER_LENGTH, 0);

    close(sockfd);
}
