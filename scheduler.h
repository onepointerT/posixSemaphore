// Copyright (C) 2026 The OnePointer Authors.
//

#ifndef _SCHEDULER_H_
#define _SCHEDULER_H_

#ifdef __cplusplus
extern "C" {
#endif


#include "mutex.h"
#include "psync.h"
#include "scheduler.h"
#include "thread.h"
#include "threadmem.h"
#include "threadingpool.h"


#define SCHEDULE_MAX_SIZE 56

#define SCHEDULE_MAX_WAITING_TIME_SECONDS 3
#define SCHEDULE_MAX_WAITING_TIME_NANOSECONDS 0
#define SCHEDULE_MAX_PROLONGANCE_TIMES 15
#define SCHEDULE_MEDIAN_WAITING_TIME timespec_new( \
                                         SCHEDULE_MAX_WAITING_TIME_SECONDS \
                                       , SCHEDULE_MAX_WAITING_TIME_NANOSECONDS );

#define SCHEDULE_MAX_MUTEXES 31
#define SCHEDULE_NUMBER_MUTEXES SCHEDULE_MAX_MUTEXES

typedef struct scheduler {
    mutex_t* lock;

    mutex_t** mtx;
    char** mtx_names;

    memory_t* mem_shared;
    memory_t* mem_private;

    struct timespec** schedule;
    threading_queue_t* threads;
    pthread_rwlock_t thread_access;

    threading_node_t* current;
    signal_queue_t* sigqueue;

    sync_process_handler_t sync_processes;

} scheduler_t;

struct scheduler* scheduler_new()


#ifdef __cplusplus
}
#endif



#endif // _SCHEDULER_H_