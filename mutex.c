// Copyright (C) 2026 The OnePointer Authors.
//

#include "mutex.h"

#include <stdlib.h>

struct mutex* mutex_new() {
    struct mutex* mtx = (struct mutex*) malloc(sizeof(struct mutex));

    mtx->locked = false;
    mtx->process_id = 0;
    mtx->try_lock = &mutex_try_lock;
    mtx->try_unlock = &mutex_try_unlock;

    return mtx;
}

bool try_lock( struct mutex* mtx, const PID processid ) {
    if ( mtx->locked && mtx->process_id != processid ) return false;
    else if ( mtx->locked && mtx->process_id == processid ) return true;
    if ( mtx->process_id != processid || mtx->process_id != 0 ) return false;

    mtx->locked = true;
    mtx->process_id = processid;
    
    return true;
}

bool try_unlock( struct mutex* mtx, const PID processid ) {
    if ( ! mtx->locked ) return true;
    else if ( mtx->locked && mtx->process_id == processid ) {
        mtx->process_id = 0;
        mtx->locked = false;
        return true;
    } return false;
}