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

// void searchServer(char **searchstr, struct node *nd, int elno, int id) {
//     if (strcmp(nd->name, searchstr[0]) == 0) {
//         if(elno == 1) {
//             res->len += 1;
//             res->list = (struct node **)realloc(sizeof(struct node *) * res->len);
//             res->list[res->len - 1] = nd;
//         }
//     }
//     else {
//         for (int i = 0; i < nd->no_child; i++) {
//             searchServer(&searchstr[1], );
//         }
//     }
// }
