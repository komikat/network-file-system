#include "./../defs.h"
#include "naming.h"

#define BACKLOG 10
#define MAX_STORES 32

int storages[MAX_STORES];
int no_stores = 0;

void *storage_listener(void *sfd_storag) {
    int sfd_storage = *((int *)sfd_storag);
    for (;;) {
        struct sockaddr_storage store_addr;
        socklen_t addr_size = sizeof store_addr;
        int new_fd;
        if ((new_fd = accept(sfd_storage, (struct sockaddr *)&store_addr,
                             &addr_size)) == -1) {
            fprintf(stderr, "Issues accepting Storage reqs: %d\n", errno);
            exit(1);
        };

        storages[no_stores] = new_fd;
        no_stores += 1;
        pthread_t newstorage;
        pthread_create(&newstorage, NULL, storage_handler, &new_fd);
        pthread_join(newstorage, NULL);
    }
}

void *client_listener(void *sfd_clien) {
    int sfd_client = *((int *)sfd_clien);
    for (;;) {
        struct sockaddr_storage client_addr;
        socklen_t addr_size = sizeof client_addr;
        int new_fd;
        if ((new_fd = accept(sfd_client, (struct sockaddr *)&client_addr,
                             &addr_size)) == -1) {
            fprintf(stderr, "Issues accepting Storage reqs: %d\n", errno);
            exit(1);
        };

        // storages[no_stores] = new_fd;
        // no_stores += 1;
        // pthread_t newstorage;
        // pthread_create(&newstorage, NULL, storage_handler, &new_fd);
        // pthread_join(newstorage, NULL);
    }
}

void *storage_handler(void *sock) {
    int sockfd = *((int *)sock);
    int receiving = 1;
    while (receiving) {
        char buf[1024] = {0};
        int r;
        if ((r = recv(sockfd, buf, 1024, 0)) == -1) {
            fprintf(stderr, "Issues recv reqs: %d\n", errno);
            exit(1);
        };

        if (strcmp(buf, "STOP") == 0) receiving = 0;
        else if (r > 0) printf("%s\n", buf);
    }
    printf("Recieved\n");
}

int main() {
    int sfd_client = initserver("localhost", PORT_NSC);
    int sfd_storage = initserver("localhost", PORT_NSS);
    // keep accepting requests

    pthread_t storage_listen, client_listen;
    pthread_create(&storage_listen, NULL, storage_listener, &sfd_storage);
    pthread_create(&client_listen, NULL, client_listener, &sfd_client);
    pthread_join(storage_listen, NULL);
    pthread_join(client_listen, NULL);

    close(sfd_client);
    close(sfd_storage);
}
