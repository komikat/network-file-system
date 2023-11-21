#include "./../defs.h"

#define BACKLOG 10
char CLIENT_BUFFER[CLIENT_BUFFER_LENGTH];
int expect_storage;

// get characters into the client buffer with client buffer length
// remove trailing \n
void fgeter() {
    fgets(CLIENT_BUFFER, CLIENT_BUFFER_LENGTH, stdin);
    CLIENT_BUFFER[strcspn(CLIENT_BUFFER, "\n")] = 0;
}

void sender(int sockfd,char * buf, int len) {
    if ((send(sockfd, buf, len, 0)) == -1) {
        fprintf(stderr, "Issues sending req: %d\n", errno);
        exit(0);
    };
}

int main() {
    int sockfd = initconn("localhost", PORT_NSC);
    while(1){
        printf("%s", "Message: ");
        fgeter();
        if(!strcmp(CLIENT_BUFFER,"exit")){
            printf("Exiting...");
            exit(0);
        }
        printf("%s\n", CLIENT_BUFFER);
        char * msg;
        char * msg_real;
        char * saveptr;

        strcpy(msg,CLIENT_BUFFER);
        strcpy(msg,saveptr);
        strcpy(msg,msg_real);
        char *token = strtok_r(msg," ",&saveptr);
        if(!strcmp(token,"CREATE") || !strcmp(token,"DELETE") || !strcmp(token,"GETINFO") || !strcmp(token,"COPY")){
            expect_storage = 0;
        }
        else if(!strcmp(token,"READ") || !strcmp(token,"WRITE") ||  !strcmp(token,"LIST")){
            expect_storage = 1;
        }else{
            printf("ERROR: UNRECOGNIZED COMMAND. RE-Enter\n");
            continue;
        }
        sender(sockfd, msg, strlen(msg));
        recver(sockfd, CLIENT_BUFFER, CLIENT_BUFFER_LENGTH, 0);
        if(!expect_storage){                                                                                                 // no need to contact storage server
            printf("%s\n", CLIENT_BUFFER);
            if(strchr(CLIENT_BUFFER,'\n')){
                char option[2] = "\0\0";
                scanf("%s", option);
                sender(sockfd,option,strlen(option));
                recver(sockfd, CLIENT_BUFFER, CLIENT_BUFFER_LENGTH, 0);
                printf("%s\n", CLIENT_BUFFER);
            }
        }else{                                                                                                                  //contacting storage server
            printf("%s\n", CLIENT_BUFFER);
            if(strchr(CLIENT_BUFFER,'\n')){
                char option[2] = "\0\0";
                scanf("%s", option);
                sender(sockfd,option,strlen(option));
                recver(sockfd, CLIENT_BUFFER, CLIENT_BUFFER_LENGTH, 0);
                printf("%s\n", CLIENT_BUFFER);
            }
                                                                                                                                        // now to send to storage server
                                                                                                         //TODO : read needs to send file size as well
            int storagesock = initconn("CLIENT_BUFFER", PORT_SSC);
            sender(storagesock, msg_real, strlen(msg_real));
            recver(storagesock, CLIENT_BUFFER, CLIENT_BUFFER_LENGTH, 0);
            close(storagesock);
        }
        printf("OPERATION COMPLETE, ON TO THE NEXT!\n");
    }
    close(sockfd);
}
