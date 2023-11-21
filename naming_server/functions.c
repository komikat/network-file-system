#include "./../defs.h"
#include "./naming.h"

// Function to print tree
void printTree(struct node *nod) {
    printf("%s\n", nod->name);
    if (nod->type == 1) {
        for (int i = 0; i < nod->no_child; i++) {
            printTree(nod->children[i]);
        }
    }
}

struct node **searchServer(char *searchstr, int id) {
    
}
