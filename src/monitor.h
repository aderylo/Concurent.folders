#pragma once
#include <stdbool.h>
#include <sys/types.h>

typedef struct readwrite readwrite;

// initialize rw - lock
void init(struct readwrite *rw);

// create brand new rw lock
readwrite *readwrite_new();

// destory rw - lock
void destroy(struct readwrite *rw);

// rw - lock abstract methods:
void BeginWrite(struct readwrite *rw);
void EndWrite(struct readwrite *rw);
void BeginRead(struct readwrite *rw);
void EndRead(struct readwrite *rw);
