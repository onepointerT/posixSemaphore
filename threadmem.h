// Copyright (C) 2026 The OnePointer Authors.
//

#ifndef _THREADMEM_H_
#define _THREADMEM_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "mutex.h"
#include "semaphore_type.h"

#include <pthread.h>


typedef struct Memory {
    void* mem;
    size_t memsize;
    char* memname;
    
    enum PTHREAD_PROCESS_AVAILABILITY pshared;
    mutex_t* mtx;

    pthread_barrier_t barrier;
    pthread_barrierattr_t barrier_attr;

    PID owner;
    semaphore_t* sem;

    pthread_cond_t cond;
} memory_t;


struct Memory* memory_new( const enum PTHREAD_PROCESS_AVAILABILITY );

#ifdef __cplusplus
}
#endif


#endif // _THREADMEM_H_