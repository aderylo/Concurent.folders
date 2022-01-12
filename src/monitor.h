#pragma once
#include <pthread.h>

struct readwrite
{
    pthread_mutex_t lock;
    pthread_cond_t readers;
    pthread_cond_t writers;
    int rcount, wcount, rwait, wwait;
    int change;
};

// initialize rw - lock
void init(struct readwrite *rw);

// destory rw - lock
void destroy(struct readwrite *rw);

// rw - lock abstract methods:
void BeginWrite(struct readwrite *rw);
void EndWrite(struct readwrite *rw);
void BeginRead(struct readwrite *rw);
void EndRead(struct readwrite *rw);
