#include "search.h"


rresult rsearch(char *searchstr, struct node *root, rresult final) {
    // printf("searching for: %s, root: %s\n", searchstr, root->path);
    if (root != NULL && strlen(root->path) > strlen(searchstr) && strstr(root->path, searchstr) != NULL) {
        rresult tmp = (rresult) malloc(sizeof(struct r_result_));
        if (tmp == NULL) {
            exit(EXIT_FAILURE);
        }

        tmp->found = root;
        tmp->next = NULL;

        if (final == NULL) {
            final = tmp;
        } else {
            rresult current = final;
            while (current->next != NULL) {
                current = current->next;
            }
            current->next = tmp;
        }
    }

    for (int i = 0; root != NULL && i < root->no_child; i++) {
        final = rsearch(searchstr, root->children[i], final);
    }

    return final;
}

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

