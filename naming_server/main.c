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

typedef struct client_args {
    int client_idx;
    int client_sock;
    char address[INET6_ADDRSTRLEN];
} *ctargs;

void *client_handler(void *args) {
    int client_idx = ((struct client_args *) args)->client_idx;
    int client_sock = ((struct client_args *) args)->client_sock;
    char *client_address = ((struct client_args *) args)->address;

    printf("%s connected.\n", client_address);

    for (;;) {
        recver(client_sock, CLIENT_BUFFER, CLIENT_BUFFER_LENGTH, 0);

        printf("(%s): %s\n", client_address, CLIENT_BUFFER);
        if (strcmp(CLIENT_BUFFER, "END") == 0) {
            printf("Client ended connection.\n");
            close(client_sock);
            int ret = 1;
            pthread_exit(&ret);
        }
        // handle all other transactions here
        // == begin transation
        // == end transaction

        strcpy(CLIENT_BUFFER, "SERVER REPLY");
        send(client_sock, CLIENT_BUFFER, CLIENT_BUFFER_LENGTH, 0);
    }

}

void *get_in_addr(struct sockaddr *sa) {
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in *) sa)->sin_addr);
    }

    return &(((struct sockaddr_in6 *) sa)->sin6_addr);
}


void *client_listener(void *sfd_client_pass) {
    int main_listener = *((int *) sfd_client_pass);

    struct sockaddr_storage client_addr;
    socklen_t addrlen;

    struct pollfd client_fds[1];
    client_fds[0].fd = main_listener;
    client_fds[0].events = POLLIN;

    int client_count = 0;
    pthread_t client_thread[MAX_CLIENT];


    for (;;) {
        // wait for new connections
        // if new connection
        // check if listener
        // if listener => create a new thread
        int poll_count = poll(client_fds, 1, -1);
        int client_new_fd = accept(main_listener, (struct sockaddr *) &client_addr, &addrlen);

        ctargs tmp = (ctargs) malloc(sizeof(struct client_args));
        tmp->client_idx = client_count;
        tmp->client_sock = client_new_fd;
        char *res = inet_ntop(client_addr.ss_family,
                              get_in_addr((struct sockaddr *) &client_addr), tmp->address, INET6_ADDRSTRLEN);


        if (res == NULL)
            strcpy(tmp->address, "::1:");


        pthread_create(&client_thread[client_count], NULL, client_handler, (void *) tmp);
        client_count++;

        if (tmp->client_idx == MAX_CLIENT) {
            break;
        }
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
    int sockfd = *((int *) sock);
    int r;
    char buf[1024] = {0};
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
                struct node *nd = (struct node *) malloc(sizeof(struct node));
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

                if (nd->parent != NULL)
                    printf("%d %o %lld %s Parent: %s\n", nd->type, nd->perms,
                           nd->size, nd->name, nd->parent->name);
                else
                    printf("%d %o %lld %s Parent: NULL\n", nd->type, nd->perms,
                           nd->size, nd->name);
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

struct node **searchServer(char *searchstr, int id) {

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
