// Copyright (C) 2026 The OnePointer Authors.
//

#ifndef _SEMAPHORE_TYPE_H_
#define _SEMAPHORE_TYPE_H_

#ifdef __cplusplus
extern "C" {
#endif


#include <stdbool.h>
#include <stdint.h>

#include "mutex.h"
#include "semaphore.h"


enum PTHREAD_PROCESS_AVAILABILITY {
    PRIVAT = PTHREAD_PROCESS_PRIVATE,
    SHARED = PTHREAD_PROCESS_SHARED
};


typedef struct semaphore {
    sem_t* sem;
    enum PTHREAD_PROCESS_AVAILABILITY pshared;
    mutex_t* mtx;

    arch_sem_t* data;
    mutex_t* dmtx;

    PID owner;
    char* owning_reason;
    unsigned long aquired;

    bool (*timedwait)( struct semaphore* sm, const struct timespec* abs_timeout );
    bool (*trywait)( struct semaphore* sm );
    sem_t* (*initsem)( struct semaphore* sm, const char* name, unsigned int value );
    void (*close)( struct semaphore* sm, const char* name );

    bool (*aquire)( struct semaphore* sm, const PID process_id, const char* owning_reason );
    bool (*release)( struct semaphore* sm, const PID process_id );

    bool (*lock)( struct semaphore* sm, const PID process_id );
    bool (*unlock)( struct semaphore* sm, const PID process_id );

    bool (*data_lock)( struct semaphore* sm, const PID process_id );
    bool (*data_unlock)( struct semaphore* sm, const PID process_id );
} semaphore_t;


bool semaphore_timedwait( struct semaphore* sm, const struct timespec* abs_timeout );
bool semaphore_trywait( struct semaphore* sm );
sem_t* semaphore_initsem( struct semaphore* sm, const char* name, unsigned int value );
bool semaphore_close( struct semaphore* sm, const char* name );

struct semaphore* semaphore_type_new( void* data, enum PTHREAD_PROCESS_VISIBILITY pshared );

bool semaphore_aquire( struct semaphore* sm, const PID process_id, const char* owning_reason );
bool semaphore_release( struct semaphore* sm, const PID process_id );

bool semaphore_lock( struct semaphore* sm, const PID process_id );
bool semaphore_unlock( struct semaphore* sm, const PID process_id );

bool semaphore_lockdata( struct semaphore* sm, const PID process_id );
bool semaphore_unlockdata( struct semaphore* sm, const PID process_id );


#ifdef __cplusplus
}
#endif


#endif // _SEMAPHORE_TYPE_H_