#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <stdbool.h>
#include "err.h"

struct monitor
{
    pthread_mutex_t lock;
    pthread_cond_t read;
    pthread_cond_t write;
    int write_wait, write_count;
    int read_wait, read_count, woke_read;
    bool woke_write;
};

void init(struct monitor *rw)
{
    int err;
    if ((err = pthread_mutex_init(&rw->lock, NULL)) != 0)
        syserr(err, "mutex init failed"); // mutex = 1
    if ((err = pthread_cond_init(&rw->read, NULL)) != 0)
        syserr(err, "cond init 1 failed");
    if ((err = pthread_cond_init(&rw->write, NULL)) != 0)
        syserr(err, "cond init 2 failed");

    rw->write_count = 0, rw->write_wait = 0, rw->woke_write = false;
    rw->read_count = 0, rw->read_wait = 0, rw->woke_read = 0;
}

struct monitor *monitor_new()
{
    struct monitor *monitor = malloc(sizeof(struct monitor));
    init(monitor);
    return monitor;
}

void destroy(struct monitor *rw)
{
    int err;

    if ((err = pthread_cond_destroy(&rw->read)) != 0)
        syserr(err, "cond destroy 1 failed");
    if ((err = pthread_cond_destroy(&rw->write)) != 0)
        syserr(err, "cond destroy 2 failed");
    if ((err = pthread_mutex_destroy(&rw->lock)) != 0)
        syserr(err, "mutex destroy failed");
}

void lock(pthread_mutex_t *mutex)
{
    int err;
    if ((err = pthread_mutex_lock(mutex)) != 0)
        syserr(err, "lock failed");
}

void unlock(pthread_mutex_t *mutex)
{
    int err;
    if ((err = pthread_mutex_unlock(mutex)) != 0)
        syserr(err, "unlock failed");
}

void wait(pthread_cond_t *cond, pthread_mutex_t *mutex)
{
    int err;
    if ((err = pthread_cond_wait(cond, mutex)) != 0)
        syserr(err, "cond wait failed");
}

void signal(pthread_cond_t *cond)
{
    int err;
    if ((err = pthread_cond_signal(cond)) != 0)
        syserr(err, "cond signal failed");
}

void broadcast(pthread_cond_t *cond)
{
    int err;
    if ((err = pthread_cond_broadcast(cond)) != 0)
        syserr(err, "cond signal failed");
}

void reader_initial(struct monitor *m)
{
    lock(&m->lock);
    while (m->write_wait > 0 || m->write_count > 0)
    {
        m->read_wait++;
        wait(&(m->read), &(m->lock));
        m->read_wait--;
        if (m->woke_read > 0)
        {
            m->woke_read--;
            break;
        }
    }

    m->read_count++;
    unlock(&m->lock);
}

void reader_final(struct monitor *m)
{
    lock(&m->lock);
    m->read_count--;
    if (m->read_count == 0 && m->write_count == 0 && m->write_wait > 0)
    {
        m->woke_write = true;
        signal(&m->write);
    }
    else if (m->write_count == 0 && m->read_count == 0)
    {
        m->woke_read = m->read_wait;
        broadcast(&m->read);
    }

    unlock(&m->lock);
}

void writer_initial(struct monitor *m)
{
    lock(&m->lock);
    while (m->write_count > 0 || m->read_count > 0 || m->write_wait > 0 || m->read_wait > 0)
    {
        m->write_wait++;
        wait(&m->write, &m->lock);
        m->write_wait--;
        if (m->woke_write)
        {
            m->woke_write = false;
            break;
        }
    }
    m->write_count++;
    unlock(&m->lock);
}

void writer_final(struct monitor *m)
{
    lock(&m->lock);
    m->write_count--;
    if (m->read_wait > 0 && m->write_count == 0 && m->read_count == 0)
    {
        m->woke_read = m->read_wait;
        broadcast(&m->read);
    }
    else if (m->write_wait > 0 && m->write_count == 0 && m->read_count == 0)
    {
        m->woke_write = true;
        signal(&m->write);
    }
    unlock(&m->lock);
}