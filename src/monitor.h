#pragma once
#include <stdbool.h>
#include <sys/types.h>

typedef struct monitor monitor;

// initialize rw - lock
void init(struct monitor *rw);

// create brand new rw lock
monitor *monitor_new();

// destory rw - lock
void destroy(struct monitor *rw);

// rw - lock abstract methods:
void writer_initial(struct monitor *rw);
void writer_final(struct monitor *rw);
void reader_initial(struct monitor *rw);
void reader_final(struct monitor *rw);