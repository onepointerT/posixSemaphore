// Copyright (C) 2026 The OnePointer Authors.
//

#include "semaphore_type.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

struct timespec* timespec_new( const time_t seconds, const long nanoseconds ) {
    struct timespec* tsp = (struct timespec*) malloc(sizeof(struct timespec));

    tsp->tv_sec = seconds;
    tsp->tv_nsec = nanoseconds;

    return tsp;
}


bool semaphore_timedwait( struct semaphore* sm, const struct timespec* abs_timeout ) {
    if ( sm == NULL || abs_timeout != NULL ) return false;
    else if ( sm->sem == NULL ) return false;
    return sem_timedwait( sm->sem, abs_timeout ) == 0;
}

bool semaphore_trywait( struct semaphore* sm ) {
    if ( sm == NULL ) return false;
    else if ( sm->sem == NULL ) return false;
    return sem_trywait( sm->sem ) == 0;
}

sem_t* semaphore_initsem( struct semaphore* sm, const char* name, unsigned int value ) {
    if ( sm == NULL ) return NULL;
    sem_t* sem = sem_open( name, _O_CREAT, _MODE_T_, value );
    if ( sem == NULL ) return NULL;
    else if ( sem_init( sem, sm->pshared, value ) != 0 ) return NULL;
    else if ( ! sm->mtx->locked && sm->owner == 0 ) sm->sem = sem;
    return sem;
}

bool semaphore_close( struct semaphore* sm, const char* name ) {
    if ( sem_unlink( name ) != 0 ) return false;
    else if ( sm->sem != NULL && ! sm->mtx->locked) {
        if ( sem_close( sm->sem ) == 0 ) {
            sm->sem = NULL;
            return true;
        } else return false;
    } return false;
}

struct semaphore* semaphore_type_new( void* data, enum PTHREAD_PROCESS_VISIBILITY pshared ) {
    struct semaphore* smt = (struct semaphore*) malloc(sizeof(struct semaphore));

    smt->sem = semaphore_initsem( smt, "main", 0 );
    smt->pshared = pshared;
    smt->mtx = mutex_new();
    
    smt->data = (arch_sem_t*) malloc(sizeof(arch_sem_t));
    smt->data->handle = data;
    smt->dmtx = mutex_new();

    smt->owner = 0;
    smt->owning_reason = "";
    smt->aquired = 0;

    smt->timedwait = &semaphore_timedwait;
    smt->trywait = &semaphore_trywait;
    smt->initsem = &semaphore_initsem;
    smt->close = &semaphore_close;
    smt->aquire = &semaphore_aquire;
    smt->release = &semaphore_release;
    smt->lock = &semaphore_lock;
    smt->unlock = &semaphore_unlock;
    smt->data_lock = &semaphore_lockdata;
    smt->data_unlock = &semaphore_unlockdata;

    return smt;
}


bool semaphore_aquire( struct semaphore* sm, const PID process_id, const char* owning_reason ) {
    if ( sm == NULL ) return false;
    else if ( sm->owner != 0 && sm->mtx->locked ) return false;
    else if ( sm->owner != 0 && ! sm->mtx->locked ) { ++sm->aquired; return true; }
    sm->owner = process_id;
    strcpy( sm->owning_reason, owning_reason );
    ++sm->aquired;

    return sm->initsem( sm, owning_reason, 1 ) != NULL;
}

bool semaphore_release( struct semaphore* sm, const PID process_id ) {
    if ( sm == NULL ) return false;
    else if ( sm->owner != process_id || sm->mtx->locked ) return false;
    else if ( sm->owner != process_id && ! sm->mtx->locked ) { --sm->aquired; return true; }
    sm->close( sm, sm->owning_reason );
    sm->owner = 0;
    sm->owning_reason = "";
    --sm->aquired;
    return sm->aquired == 0;
}

bool semaphore_lock( struct semaphore* sm, const PID process_id ) {
    if ( sm == NULL ) return false;
    else if ( sm->owner == 0 && ! sm->mtx->locked ) {
        if ( sm->aquired == 0 ) {
            if ( ! sm->aquire( sm, process_id, "sm_lock" ) ) return false;
        }
        return sm->mtx->try_lock( sm->mtx, process_id );
    } return false;
}

bool semaphore_unlock( struct semaphore* sm, const PID process_id ) {
    if ( sm == NULL ) return false;
    else if ( sm->owner != process_id ) return false;
    else if ( sm->mtx->locked ) {
        return sm->mtx->try_unlock( sm->mtx, process_id );
    } return false;
}

bool semaphore_lockdata( struct semaphore* sm, const PID process_id ) {
    if ( sm == NULL ) return false;
    else if ( sm->dmtx->process_id != 0 ) return false;
    return sm->dmtx->try_lock( sm->dmtx, process_id );
}

bool semaphore_unlockdata( struct semaphore* sm, const PID process_id ) {
    if ( sm == NULL ) return false;
    else if ( sm->dmtx->process_id != process_id ) return false;
    return sm->dmtx->try_unlock( sm->dmtx, process_id );
}
