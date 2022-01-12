#include "monitor.h"

#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

#include "err.h"

struct readwrite
{
    pthread_mutex_t lock;
    pthread_cond_t readers;
    pthread_cond_t writers;
    int rcount, wcount, rwait, wwait;
    int change;
};

void init(struct readwrite *rw)
{
    int err;
    if ((err = pthread_mutex_init(&rw->lock, NULL)) != 0)
        syserr(err, "mutex init failed"); // mutex = 1
    if ((err = pthread_cond_init(&rw->readers, 0)) != 0)
        syserr(err, "cond init 1 failed");
    if ((err = pthread_cond_init(&rw->writers, 0)) != 0)
        syserr(err, "cond init 2 failed");

    rw->rcount = 0;
    rw->wcount = 0;
    rw->rwait = 0;
    rw->wwait = 0;
}

readwrite *readwrite_new()
{
    readwrite *rw = malloc(sizeof(readwrite));
    if (!rw)
        return NULL;
    memset(rw, 0, sizeof(readwrite));
    init(rw);
    return rw;
}

void destroy(struct readwrite *rw)
{
    int err;

    if ((err = pthread_cond_destroy(&rw->readers)) != 0)
        syserr(err, "cond destroy 1 failed");
    if ((err = pthread_cond_destroy(&rw->writers)) != 0)
        syserr(err, "cond destroy 2 failed");
    if ((err = pthread_mutex_destroy(&rw->lock)) != 0)
        syserr(err, "mutex destroy failed");

    free(rw);
}

void BeginWrite(struct readwrite *rw)
{
    int err;
    if ((err = pthread_mutex_lock(&rw->lock)) != 0)
        syserr(err, "lock failed");

    if (rw->wcount == 1 || rw->rcount > 0)
    {
        ++rw->wwait;
        while (rw->change != 1)
            if ((err = pthread_cond_wait(&rw->writers, &rw->lock)) != 0)
                syserr(err, "cond wait failed");
        --rw->wwait;
        rw->change = 0;
    }

    rw->wcount = 1;

    if ((err = pthread_mutex_unlock(&rw->lock)) != 0)
        syserr(err, "unlock failed");
}

void EndWrite(struct readwrite *rw)
{
    int err;
    if ((err = pthread_mutex_lock(&rw->lock)) != 0)
        syserr(err, "lock failed");

    rw->wcount = 0;

    if (rw->rwait > 0)
    {
        rw->change = 1;
        if ((err = pthread_cond_signal(&rw->readers)) != 0)
            syserr(err, "cond signal failed");
    }
    else
    {
        rw->change = 1;
        if ((err = pthread_cond_signal(&rw->writers)) != 0)
            syserr(err, "cond signal failed");
    }

    if ((err = pthread_mutex_unlock(&rw->lock)) != 0)
        syserr(err, "unlock failed");
}

void BeginRead(struct readwrite *rw)
{
    int err;
    if ((err = pthread_mutex_lock(&rw->lock)) != 0)
        syserr(err, "lock failed");

    if (rw->wcount == 1 || rw->rwait > 0)
    {
        rw->rwait++;
        while (rw->change != 1)
            if ((err = pthread_cond_wait(&rw->readers, &rw->lock)) != 0)
                syserr(err, "cond wait failed");

        rw->change = 0;
        rw->rwait--;
    }

    rw->rcount++;
    if ((err = pthread_cond_signal(&rw->readers)) != 0)
        syserr(err, "cond signal failed");

    if ((err = pthread_mutex_unlock(&rw->lock)) != 0)
        syserr(err, "unlock failed");
}

void EndRead(struct readwrite *rw)
{
    int err;
    if ((err = pthread_mutex_lock(&rw->lock)) != 0)
        syserr(err, "lock failed");

    if (--rw->rcount == 0)
    {
        rw->change = 1;
        if ((err = pthread_cond_signal(&rw->writers)) != 0)
            syserr(err, "cond signal failed");
    }

    if ((err = pthread_mutex_unlock(&rw->lock)) != 0)
        syserr(err, "unlock failed");
}
