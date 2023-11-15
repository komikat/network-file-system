struct node {
    char *name;
    uint size;
    int type;
    struct node *parent;
    struct node **children;
};

int send_error(int, int, char *);