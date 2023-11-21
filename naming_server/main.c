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

// Function to initialize tree for a storage server
// and keep receiving its messages
void *storage_handler(void *sock) {
    int sockfd = *((int *)sock);
    int r;
    char buf[1024] = {0};
    int receiving = 1;
    int locked = 1;
    struct node *parent = NULL;

    // Loop that receives and initializes tree
    while (receiving) {

        // Buffer recieved
        if ((r = recv(sockfd, buf, 1024, 0)) == -1) {
            fprintf(stderr, "Issues recv reqs: %d\n", errno);
            exit(1);
        };

        // Check if over
        if (strcmp(buf, "STOP") == 0) {
            receiving = 0;
            locked = 0;
        }

        // if not over, get data and make node
        else if (r > 0) {

            // delimiter if multiple nodes in buffer
            char delim[] = ";";
            char * tok = strtok(buf, delim);
            char namebuf[1024];

            // for every node in buffer
            while(tok != NULL) {

                // make node
                struct node *nd = (struct node *)malloc(sizeof(struct node));
                sscanf(tok, "%d %d %lld %s", &nd->type, &nd->perms, &nd->size, namebuf);
                int pos = 0, parentpos = 0;
                for (int i = 0; i < strlen(namebuf); i++){
                    if (namebuf[i] == '/') {
                        parentpos = pos;
                        pos = i;
                    }
                }
                char *name = (char *)malloc(sizeof(char) * strlen(&namebuf[pos+1]) + 2);
                strcpy(name, &namebuf[pos+1]);
                nd->name = name;
                nd->no_child = 0;
                nd->parent = NULL;
                nd->children = NULL;

                // find parent and assign
                while(1) {
                    if (parent == NULL) {
                        parent = nd;
                        // printf("First parent %s\n", parent->name);
                        break;
                    }
                    else {
                        // printf("Comparing: %s, %s, %d\n", parent->name, &(namebuf[parentpos + 1]), strlen(parent->name));
                        if (strncmp(parent->name, &(namebuf[parentpos + 1]), strlen(parent->name)) == 0 && parent->type == 1) {
                            nd->parent = parent;
                            parent->no_child += 1;
                            parent->children = (struct node **)realloc(parent->children, sizeof(struct node *) * parent->no_child);
                            parent->children[parent->no_child - 1] = nd;
                            break;
                        }
                        else parent = parent->parent;
                    }
                }

                if (nd->type == 1) parent = nd;
                else parent = nd->parent;

                if (nd->parent != NULL) printf("%d %o %lld %s Parent: %s\n", nd->type, nd->perms, nd->size, nd->name, nd->parent->name);
                else printf("%d %o %lld %s Parent: NULL\n", nd->type, nd->perms, nd->size, nd->name);
                tok = strtok(NULL, delim);
            }
        }
    }
    printf("Recieved\n");
    while (1) {
        if ((r = recv(sockfd, buf, 1024, 0)) == -1) {
            fprintf(stderr, "Issues recv reqs: %d\n", errno);
            exit(1);
        };
        printf("Buffer: %s\n", buf);
    }
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
