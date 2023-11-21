#include "search.h"

typedef struct r_result_ {
    struct node *found;
    struct r_result_ *next;
} *rresult;

rresult rsearch(char *searchstr, struct node *root, rresult final) {
    if (strstr(root->path, searchstr) != NULL) {
        rresult tmp = (rresult) malloc(sizeof(struct r_result_));

        tmp->found = root;
        tmp->next = NULL;

        final->next = tmp;
    }

    for (int i = 0; i < root->no_child; i++) {
        rsearch(searchstr, root, final);
    }

    return final;
}