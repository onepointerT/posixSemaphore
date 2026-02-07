// Copyright (C) The OnePointer Authors.
//

#include "threading.h"

#include <stdlib.h>

struct PThread* thread_new( const enum PTHREAD_PROCESS_AVAILABILITY pshared
                    , const int cpus_used, const size_t size_children_arr
                    , const size_t size_data_arr, const size_t size_semaphore_arr
                    , struct PThread* prev, struct PThread* next
                    , struct PThread* parent, const enum PTHREAD_CANCEL_STATE pcstate
                    , const char* threadname, const enum PTHREAD_SCHED sched_type
                    , const int sched_priority, const size_t stacksize
                    , const enum PTHREAD_MUTEX mutexkind
                    , process_f func, void* args
) {
    struct PThread* pth = (struct PThread*) malloc(sizeof(struct PThread));

    pth->pshared = pshared;
    pth->name = (char*) malloc(sizeof(char)*strlen(threadname));
    strcpy( pth->name, threadname );
    pth->proc = pthread_new( pshared, stacksize, threadname
                        , sched_priority, sched_type, pth, func, args );
    pth->handle = pthread_getw32threadhandle_np( *pth->proc );
    pth->threadid = pthread_getw32threadid_np( *pth->proc );
    
    pth->attr_barrier = NULL;
    pthread_barrierattr_init( pth->attr_barrier );
    pthread_barrierattr_setpshared( pth->attr_barrier, pshared );

    pthread_barrier_init( pth->barrier, pth->attr_barrier, PTHREAD_MAX_BARRIER_ARRAY );

    pth->attr_mutex = NULL;
    pthread_mutexattr_init( pth->attr_mutex );
    pthread_mutexattr_setpshared( pth->attr_mutex, pshared );
    pthread_mutexattr_setkind_np( pth->attr_mutex, mutexkind );
    
    pth->mutex = NULL;
    pthread_mutex_init( pth->mutex, pth->attr_mutex );
    
    pth->process_memory = memory_new( pshared );

    pth->children = (struct PThread**) malloc(sizeof(struct PThread*)*PTHREAD_MAX_CHILDREN);
    pth->joined = NULL;
    pth->next = next;
    pth->prev = prev;
    pth->parent = parent;
    
    pth->data = (void**) malloc(sizeof(void*)*PTHREAD_MAX_DATA_ARRAY);
    pth->sem = (struct semaphore**) malloc(sizeof(struct semaphore*)*PTHREAD_MAX_SEMAPHORE);

    pth->attr_rwlock = NULL;
    pthread_rwlockattr_init( pth->attr_rwlock );
    pth->rwlock = NULL;
    pthread_rwlock_init( pth->rwlock, pth->attr_rwlock );

    pth->cpucount = pthread_num_processors_np();
    pth->cpus_used = cpus_used > pth->cpucount
                ? pth->cpucount : cpus_used;
    pth->cpu = (cpu_set_t*) malloc(sizeof(cpu_set_t)*pth->cpus_used);

    pth->stat = PThreadStateInitial;
    pth->pshared = pshared;
    pth->pcancelstate = pcstate;

    pth->func = func;
    pth->arg = args;

    return pth;
}


pthread_t* pthread_new( const enum PTHREAD_PROCESS_AVAILABILITY pshared
                    , const size_t stacksize, const char* threadname
                    , const int sched_priority, const enum PThreadSched sched_type
                    , struct PThread* thread
                    , process_f func, void* args
) {

    pthread_attr_t* pta = NULL;
    pthread_attr_init( pta );
    pthread_attr_setname_np( pta, threadname );
    pthread_attr_setstacksize( pta, stacksize );
    pthread_attr_setschedpolicy( pta, SCHED_FIFO );
    pthread_attr_setinheritsched( pta
        , sched_type == PThreadInheritSched ? TRUE : FALSE );

    pthread_t* pt = NULL;
    pthread_create( pt, pta, func, args );

    struct sched_param* sp = NULL;
    int sprio = 0;
    pthread_getschedparam( *pt, &sprio, sp );
    sp->sched_priority = sched_priority;
    pthread_attr_setschedparam( pta, sp );

    if ( thread != NULL ) {
        thread->proc = pt;
        thread->attr = pta;
    }

    return pt;
}