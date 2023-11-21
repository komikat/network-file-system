#include "./../defs.h"
#include "./naming.h"

// Function to print tree

struct node *absoluteSearch(char **searchStr, int elno, struct node *root) {
    if (strcmp(searchStr[0], root->name) == 0) {
        if (elno == 1) return root;
        struct node *test;
        for (int i = 0; i < root->no_child; i++) {
            test = absoluteSearch(&searchStr[1], elno - 1, root->children[i]);
            if (test != NULL) return test;
        }
    } else return NULL;
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
