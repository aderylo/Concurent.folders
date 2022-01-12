#pragma once
#include <pthread.h>

typedef struct readwrite readwrite;

// initialize rw - lock
void init(struct readwrite *rw);

// destory rw - lock
void destroy(struct readwrite *rw);

// rw - lock abstract methods:
void BeginWrite(struct readwrite *rw);
void EndWrite(struct readwrite *rw);
void BeginRead(struct readwrite *rw);
void EndRead(struct readwrite *rw);
