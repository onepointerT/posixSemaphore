// Copyright (C) 2026 The OnePointer Authors.
//

#ifndef _THREADING_H_
#define _THREADING_H_


#ifdef __cplusplus
extern "C" {
#endif

#include "psignals.h"
#include "semaphore_type.h"
#include "threadmem.h"

#include <pthread.h>

#include <stdbool.h>
#include <stdint.h>


typedef enum PTHREAD_STATE
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


typedef enum PTHREAD_SCHED
{
  PThreadInheritSched = 0,
  PThreadExplicitSched = 1
}
PThreadSched;


typedef enum PTHREAD_CANCEL_TYPE
{
  PTHREAD_CANCEL_ASYNCHRONOUS = 0,
  PTHREAD_CANCEL_DEFERRED = 1
}
PThreadCancelType;


typedef enum PTHREAD_CANCEL_STATE
{
  PTHREAD_CANCEL_ENABLE = 0,
  PTHREAD_CANCEL_DISABLE = 1
}
PThreadCancelState;


/*
 * Mutex types.
 */
enum PTHREAD_MUTEX
{
  /* Compatibility with LinuxThreads */
  PTHREAD_MUTEX_FAST_NP,
  PTHREAD_MUTEX_RECURSIVE_NP,
  PTHREAD_MUTEX_ERRORCHECK_NP,
  PTHREAD_MUTEX_TIMED_NP = PTHREAD_MUTEX_FAST_NP,
  PTHREAD_MUTEX_ADAPTIVE_NP = PTHREAD_MUTEX_FAST_NP,
  /* For compatibility with POSIX */
  PTHREAD_MUTEX_NORMAL = PTHREAD_MUTEX_FAST_NP,
  PTHREAD_MUTEX_RECURSIVE = PTHREAD_MUTEX_RECURSIVE_NP,
  PTHREAD_MUTEX_ERRORCHECK = PTHREAD_MUTEX_ERRORCHECK_NP,
  PTHREAD_MUTEX_DEFAULT = PTHREAD_MUTEX_NORMAL
} PThreadMutex;


/**
 * Common signal macros include:
 */
typedef enum PTHREAD_SIGNAL
{
  SIGINT = 0, //: Interrupt signal (e.g., Ctrl+C).
  SIGTERM = 1, //: Termination request.
  SIGSEGV = 2, //: Invalid memory access (segmentation fault).
  SIGABRT = 3, //: Abnormal program termination.
  SIGSUSP = 4, //: Suspend the process
  SIGILL = 5 //: Illegal operation
}
PThreadSignal;


typedef struct SigAction {

} sig_action_t;


#define PTHREAD_MAX_CHILDREN 152
#define PTHREAD_MAX_SEMAPHORE 102
#define PTHREAD_MAX_DATA_ARRAY 56
#define PTHREAD_MAX_BARRIER_ARRAY 22


typedef struct PThread {
    unsigned long threadid;
    char* name;

    void* handle;
    void* (__PTW32_CDECL *func)(void);
    void* arg;

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

    pthread_attr_t attr;
    pthread_barrierattr_t attr_barrier;
    pthread_mutexattr_t attr_mutex;

    pthread_mutex_t mutex;
    
    pthread_key_t* key;
    void** data;
    struct semaphore** sem;

    pthread_rwlock_t* rwlock;
    pthread_rwlockattr_t* attr_rwlock;

    int cpus_used;
    cpu_set_t* cpu;
    size_t cpucount;

    PThreadState stat;

    enum PTHREAD_PROCESS_AVAILABILITY pshared;
    enum PTHREAD_CANCEL_STATE pcancelstate;

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
                    , const enum PThreadMutex mutexkind
                    , void (* func)(void*), void* args );

pthread_t* pthread_new( const enum PTHREAD_PROCESS_AVAILABILITY pshared
                    , const size_t stacksize, const char* threadname
                    , const int sched_priority, const enum PThreadSched sched_type
                    , struct PThread* thread
                    , void (* func)(void*), void* args
);


bool thread_exit( struct PThread* thread, const SIGNAL sig );

#ifdef __cplusplus
}
#endif


#endif // _THREADING_H_