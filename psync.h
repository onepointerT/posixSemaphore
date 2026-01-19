// Copyright (C) The OnePointer Authors.
//

#ifndef _PSYNC_H_
#define _PSYNC_H_


#ifdef __cplusplus
extern "C" {
#endif

#include "psignals.h"
#include "semaphore.h"
#include "threading.h"


typedef struct Signal* (*callback_reentrance)(struct semaphore* dsem);
typedef struct PThread* (*callback_sync_suspense)(const struct timespec* wait_time);

typedef struct SyncTime {
    struct timespec* time_current;
    struct timespec* time_syncpoint;
    struct timespec* time_suspended;

    enum PSignals signal_in;
    enum PSignals signal_now;
    enum PSignals signal_finished;
} sync_time_t;


typedef struct SyncTimeHandler {
    struct SyncTime* st;

    callback_reentrance cb_reentrance;
    callback_sync_suspense cb_sync_suspense;
} sync_time_t;


typedef enum PSignals (*callback_sync)(const struct SyncProcess* sp);

typedef enum P_ASYNC
{
    PROCESS_ASYNCRONOUS = 0,
    PROCESS_SYNCRONOUS = 1
}
PSync;

typedef struct SyncProcess {
    struct SyncTime* p1;
    struct SyncTime* p2;

    enum PSync psync;
} sync_process_t;



typedef struct SyncProcessHandler {
    struct SyncProcess* sp;
    
    callback_sync cb_sync;
    callback_sync cb_async;
} sync_process_handler_t;


#ifdef __cplusplus
}
#endif


#endif // _PSYNC_H_