#include "./../defs.h"

#define BACKLOG 10

int main() {
    int sfd_client = initserver("localhost", PORT_NSC);
    int sfd_storage = initserver("localhost", PORT_NSS);
    // keep accepting requests
    for (;;) {
        struct sockaddr_storage client_addr;
        socklen_t addr_size = sizeof client_addr;
        int new_fd;
        if ((new_fd = accept(sfd_client, (struct sockaddr *)&client_addr,
                             &addr_size)) == -1) {
            fprintf(stderr, "Issues accepting reqs: %d\n", errno);
            exit(1);
        };

        char buf[100] = {0};
        if (recv(new_fd, buf, 100, 0) == -1) {
            fprintf(stderr, "Issues recv reqs: %d\n", errno);
            exit(1);
        };

        printf("%s\n", buf);

        char *msg = "Greetings from the server!";
        int len = strlen(msg);

        if (send(new_fd, msg, len, 0) == -1) {
            fprintf(stderr, "Issues sending reqs: %d\n", errno);
            exit(1);
        }
    }

    close(sfd_client);
    close(sfd_storage);
}
