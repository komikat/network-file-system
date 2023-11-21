#ifndef HELPER_H_
#define HELPER_H_
// #include "defs.h"

struct node {
    int perms;
    int type;
    long long int size;
    char *name;
    struct node *parent;
    int no_child;
    struct node **children;
};

struct storage {
    int id;
};
void recver(int sockfd, char *buf, int len, int flags);
int initserver(char *addr, char *port);
int initconn(char *addr, char *port);

#endif // HELPER_H_
