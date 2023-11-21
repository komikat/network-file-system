#include "./../defs.h"
#include "naming.h"

#define BACKLOG 10
// #define MAX_STORES 64

struct storage **storages = NULL;
int no_stores = 0;

// char **

char CLIENT_BUFFER[CLIENT_BUFFER_LENGTH];

void *storage_listener(void *sfd_storag) {
    int sfd_storage = *((int *)sfd_storag);
    for (;;) {
        struct sockaddr_storage store_addr;
        socklen_t addr_size = sizeof store_addr;
        int new_fd;
        if ((new_fd = accept(sfd_storage, (struct sockaddr *)&store_addr,
                             &addr_size)) == -1) {
            fprintf(stderr, "Issues accepting client reqs: %d\n", errno);
            exit(1);
        };

        pthread_t newstorage;
        pthread_create(&newstorage, NULL, storage_handler, &new_fd);
        pthread_join(newstorage, NULL);
    }
}

void *client_listener(void *sfd_client_pass) {
    int sfd_client = *((int *)sfd_client_pass);

    struct sockaddr_storage client_addr;
    socklen_t addr_size = sizeof client_addr;
    int client_new_sock;
    if ((client_new_sock = accept(sfd_client, (struct sockaddr *)&client_addr,
                                  &addr_size)) == -1) {
        fprintf(stderr, "Issues accepting client reqs: %d\n", errno);
        exit(1);
    };

    printf("new client\n");
    // recver(client_new_sock, CLIENT_BUFFER, CLIENT_BUFFER_LENGTH, 0);

    struct pollfd pfds[1];
    pfds[0].fd = client_new_sock;
    pfds[0].events = POLLIN;

    int num = poll(pfds, 1, 2500);

    if (num == 0) {
        printf("%s\n", "No requests!!!!!!");
    } else {
        int r;
        if ((r = recv(client_new_sock, CLIENT_BUFFER, CLIENT_BUFFER_LENGTH,
                      0)) == -1) {
            fprintf(stderr, "Issues recv reqs: %d\n", errno);
            exit(1);
        };
        printf("%s", CLIENT_BUFFER);
    }
}

// Function to get IP address from socket
char *getip(int sockfd) {
    struct sockaddr_storage addr;
    socklen_t addr_len = sizeof(addr);

    if (getpeername(sockfd, (struct sockaddr*)&addr, &addr_len) == 0) {
        // The socket is connected, and addr now contains the peer's address
        char *ipstr = (char *)malloc(sizeof(char) * INET6_ADDRSTRLEN);
        int port;

        if (addr.ss_family == AF_INET) {
            struct sockaddr_in *s = (struct sockaddr_in *)&addr;
            port = ntohs(s->sin_port);
            inet_ntop(AF_INET, &s->sin_addr, ipstr, sizeof ipstr);
        } else { // AF_INET6
            struct sockaddr_in6 *s = (struct sockaddr_in6 *)&addr;
            port = ntohs(s->sin6_port);
            inet_ntop(AF_INET6, &s->sin6_addr, ipstr, sizeof ipstr);
        }

        // printf("Peer IP address: %s\n", ipstr);
        return ipstr;
    } else {
        perror("Could not get IP address...\n");
    }
}

// Function to initialize tree for a storage server
// and keep receiving its messages
void *storage_handler(void *sock) {
    int sockfd = *((int *)sock);
    int r;
    int receiving = 1;
    int locked = 1;
    struct node *parent = NULL;

    // Create storage server entry
    struct storage *entry = (struct storage *)malloc(sizeof(struct storage));
    entry->id = no_stores;
    entry->red1 = -1;
    entry->red2 = -1;
    entry->ip = getip(sockfd);
    no_stores += 1;
    storages = (struct storage **)realloc(storages, sizeof(struct storage *) * no_stores);
    storages[no_stores - 1] = entry;

    // Loop that receives and initializes tree
    while (receiving) {

        // Buffer recieved
        char buf[1024] = {0};
        if ((r = recv(sockfd, buf, 1024, 0)) == -1) {
            fprintf(stderr, "Issues recv reqs: %d\n", errno);
            exit(1);
        }
        // if not over, get data and make node
        else if (r > 0) {

            // printf("Buffer: %s\n\n", buf);

            // delimiter if multiple nodes in buffer
            char delim[] = ";";
            char *tok = strtok(buf, delim);
            char namebuf[1024];

            // for every node in buffer
            while (tok != NULL) {

                // printf("Token: %s\n\n", tok);

                // Check if over
                if (strcmp(tok, "STOP") == 0) {
                    receiving = 0;
                    locked = 0;
                    break;
                }

                // make node
                struct node *nd = (struct node *)malloc(sizeof(struct node));
                sscanf(tok, "%d %d %lld %s", &nd->type, &nd->perms, &nd->size,
                       namebuf);
                int pos = 0, parentpos = 0;
                for (int i = 0; i < strlen(namebuf); i++) {
                    if (namebuf[i] == '/') {
                        parentpos = pos;
                        pos = i;
                    }
                }
                char *name = (char *)malloc(
                    sizeof(char) * strlen(&namebuf[pos + 1]) + 2);
                strcpy(name, &namebuf[pos + 1]);
                nd->name = name;
                nd->no_child = 0;
                nd->parent = NULL;
                nd->children = NULL;

                // find parent and assign
                while (1) {
                    if (parent == NULL) {
                        parent = nd;
                        entry->root = nd;
                        printf("First parent %s\n", parent->name);
                        break;
                    }
                    else {
                        printf("Comparing: %s, %s, %d\n", parent->name, &(namebuf[parentpos + 1]), strlen(parent->name));
                        if (strncmp(parent->name, &(namebuf[parentpos + 1]), strlen(parent->name)) == 0 && parent->type == 1) {
                            nd->parent = parent;
                            parent->no_child += 1;
                            parent->children = (struct node **)realloc(
                                parent->children,
                                sizeof(struct node *) * parent->no_child);
                            parent->children[parent->no_child - 1] = nd;
                            break;
                        } else
                            parent = parent->parent;
                    }
                }

                if (nd->type == 1)
                    parent = nd;
                else
                    parent = nd->parent;
                tok = strtok(NULL, delim);
            }
        }
    }
    printf("Recieved\n");
    struct node *nod = storages[entry->id]->root;
    printf("Root: %s\n", nod->name);
    printf("ID: %d\nIP: %s\n\n", entry->id, entry->ip);
    printTree(storages[entry->id]->root);
    while (1);
    // while (1) {
    //     if ((r = recv(sockfd, buf, 1024, 0)) == -1) {
    //         fprintf(stderr, "Issues recv reqs: %d\n", errno);
    //         exit(1);
    //     };
    //     printf("Buffer: %s\n", buf);
    // }
}

int main() {
    int sfd_client = initserver("localhost", PORT_NSC);
    int sfd_storage = initserver("localhost", PORT_NSS);
    // keep accepting requests

    pthread_t storage_listen, client_listen;
    // pthread_create(&storage_listen, NULL, storage_listener, &sfd_storage);
    pthread_create(&client_listen, NULL, client_listener, &sfd_client);
    pthread_join(storage_listen, NULL);
    pthread_join(client_listen, NULL);

    close(sfd_client);
    close(sfd_storage);
}
