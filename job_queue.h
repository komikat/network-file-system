#ifndef FINAL_PROJECT_37_JOB_QUEUE_H
#define FINAL_PROJECT_37_JOB_QUEUE_H

#include "defs.h"

enum Status {
    RUNNING, DONE, FAILURE, QUEUED
};
typedef struct job_struct {
    int id;
    int type;
    int server;
    int client;
    enum Status status;
    char *job_string;
    struct job_struct *next;
} job_, *job;


#endif //FINAL_PROJECT_37_JOB_QUEUE_H
