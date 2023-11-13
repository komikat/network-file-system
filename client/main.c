#include "./../defs.h"

#define BACKLOG 10
#define BUFFER 100

int main() {
    int sockfd = initconn("localhost", PORT_NSC);

    char msg[BUFFER] = {0};
    printf("%s", "Message: ");
    fgets(msg, BUFFER, stdin);

    int len = strlen(msg);
    if ((send(sockfd, msg, BUFFER, 0)) == -1) {
        fprintf(stderr, "Issues sending req: %d\n", errno);
        exit(1);
    };

    char buf[BUFFER] = {0};
    if (recv(sockfd, buf, BUFFER, 0) == -1) {
        fprintf(stderr, "Issues recv req: %d\n", errno);
        exit(1);
    }

    printf("Server: %s\n", buf);
    close(sockfd);
}
