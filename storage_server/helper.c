#include "../defs.h"
#include "storage.h"

// Sends error messages with code
// 0 on failure, 1 on success
int send_error(int sockfd, int code, char *message) {
    int len = strlen(message);
    char buffer[len+16];
    sprintf(buffer, "Error %d: %s\n", code, message);
    if ((send(sockfd, buffer, BUFFER, 0)) == -1) {
        fprintf(stderr, "Issues sending req: %d\n", errno);
        return 0;
    };
    return 1;
}