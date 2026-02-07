// Copyright (C) 2026 The OnePointer Authors.
//

#ifndef _THREADING_H_
#define _THREADING_H_


#ifdef __cplusplus
extern "C" {
#endif

#include "functionqueue.h"
#include "psignals.h"
#include "ptime.h"
#include "semaphore_type.h"
#include "threadmem.h"

#include <pthread.h>

#include <stdbool.h>
#include <stdint.h>


typedef enum
{
  /*
   * This enumeration represents the state of the thread;
   * The thread is still valid if the numeric value of the
   * state is greater or equal "PThreadStateRunning".
   */
  PThreadStateInitial = 0,	/* Thread not running                   */
  PThreadStateReuse,            /* In reuse pool.                       */
  PThreadStateRunning,		/* Thread alive & kicking               */
  PThreadStateSuspended,	/* Thread alive but suspended           */
  PThreadStateCancelPending,	/* Thread alive but                     */
                                /* has cancellation pending.            */
  PThreadStateCanceling,	/* Thread alive but is                  */
                                /* in the process of terminating        */
                                /* due to a cancellation request        */
  PThreadStateExiting,		/* Thread alive but exiting             */
                                /* due to an exception                  */
  PThreadStateLast              /* All handlers have been run and now   */
                                /* final cleanup can be done.           */
}
PThreadState;


typedef enum
{
  PThreadInheritSched = 0,
  PThreadExplicitSched = 1
}
PThreadSched;


typedef enum
{
  PThreadCancelAsyncronous = 0,
  PThreadCancelSyncronous = 1
}
PThreadCancelType;


typedef enum PTHREAD_CANCEL_STATE
{
  PThreadCancelEnable = 0,
  PThreadCancelDisable = 1
}
PThreadCancelState;


/*
 * Mutex types.
 */
typedef enum
{
  /* Compatibility with LinuxThreads */
  MUTEX_FAST_NP,
  MUTEX_RECURSIVE_NP,
  MUTEX_ERRORCHECK_NP,
  MUTEX_TIMED_NP = MUTEX_FAST_NP,
  MUTEX_ADAPTIVE_NP = MUTEX_FAST_NP,
  /* For compatibility with POSIX */
  MUTEX_NORMAL = MUTEX_FAST_NP,
  MUTEX_RECURSIVE = MUTEX_RECURSIVE_NP,
  MUTEX_ERRORCHECK = MUTEX_ERRORCHECK_NP,
  MUTEX_DEFAULT = MUTEX_NORMAL
} PThreadMutex;


/**
 * Common signal macros include:
 */
typedef enum
{
  sigint = 0, //: Interrupt signal (e.g., Ctrl+C).
  sigterm = 1, //: Termination request.
  sigsegv = 2, //: Invalid memory access (segmentation fault).
  sigabrt = 3, //: Abnormal program termination.
  sigsusp = 4, //: Suspend the process
  sigill = 5, //: Illegal operation
  sigresume = 6 //: Resume the process
}
PThreadSignal;


#define PTHREAD_MAX_CHILDREN 152
#define PTHREAD_MAX_SEMAPHORE 102
#define PTHREAD_MAX_DATA_ARRAY 56
#define PTHREAD_MAX_BARRIER_ARRAY 22


typedef void* (*process_f)(void*);


typedef struct PThread {
    unsigned long threadid;
    char* name;

    void* handle;
    process_f func;
    void* arg;

    struct FunctionQueue* fqueue;

    pthread_t* proc;
    pthread_barrier_t* barrier;

    struct Memory* process_memory;
    
    struct SignalQueue* signals;
    struct SignalQueue* signals_sent;
    
    struct PThread** children;
    struct PThread* joined;
    struct PThread* next;
    struct PThread* prev;

    struct PThread* parent;

    pthread_attr_t* attr;
    pthread_barrierattr_t* attr_barrier;
    pthread_mutexattr_t* attr_mutex;

    pthread_mutex_t* mutex;
    
    pthread_key_t* key;
    void** data;
    struct semaphore** sem;

    pthread_rwlock_t* rwlock;
    pthread_rwlockattr_t* attr_rwlock;

    int cpus_used;
    cpu_set_t* cpu;
    size_t cpucount;

    PThreadState stat;
    struct ProcessTime* time_pstarted;

    enum PTHREAD_PROCESS_AVAILABILITY pshared;
    enum PTHREAD_CANCEL_STATE pcancelstate;

    tick_t childhood_start;
    tick_t childhood_ticks_per_second;
    tick_t childhood_elapsed_ticks_since;

    bool (*detach)( struct PThread* pt );
    bool (*equal)( const struct PThread* pt1, const struct PThread* pt2 );
    int (*signal)( struct PThread* pt, const enum PThreadSignal sig );
    int (*exit)( struct PThread* pt, const enum PThreadSignal sig );
    int (*cancel)( struct PThread* pt, const bool recursive, const bool async_not_deferred );
    bool (*join)( struct PThread* pt_joinable, struct PThread* pt_joining, void** value_ptr );
    bool (*join_w_time)( struct PThread* pt_joinable, struct PThread* pt_joining, void** value_ptr, const struct timespec* abstime );

    int (*setaffinity)( struct PThread* pt, size_t cpusetsize, const cpu_set_t* cpuset );
    int (*getaffinity)( struct PThread* pt, size_t cpusetsize, cpu_set_t* cpuset );

    int (*delay)( struct PThread* pt, struct timespec* interval );

    
} thread_t;

struct PThread* thread_new( const enum PTHREAD_PROCESS_AVAILABILITY pshared
                    , const int cpus_used, const size_t size_children_arr
                    , const size_t size_data_arr, const size_t size_semaphore_arr
                    , struct PThread* prev, struct PThread* next
                    , struct PThread* parent, const enum PTHREAD_CANCEL_STATE pcstate
                    , const char* threadname, const enum PTHREAD_SCHED sched_type
                    , const int sched_priority, const size_t stacksize
                    , const enum PThreadMutex mutexkind
                    , process_f func, void* args );

pthread_t* pthread_new( const enum PTHREAD_PROCESS_AVAILABILITY pshared
                    , const size_t stacksize, const char* threadname
                    , const int sched_priority, const enum PThreadSched sched_type
                    , struct PThread* thread
                    , process_f func, void* args
);

struct timespec* thread_time( const struct PThread* thread );

bool thread_exit( struct PThread* thread, const enum PSignals sig );

#ifdef __cplusplus
}
#endif


#endif // _THREADING_H_