// Copyright (C) 2026 The OnePointer Authors.
//

#ifndef _MUTEX_H_
#define _MUTEX_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>


typedef unsigned short PID;


typedef struct mutex {
    bool locked;
    PID process_id;

    bool (*try_lock)( struct mutex* mtx, const PID processid );
    bool (*try_unlock)( struct mutex* mtx, const PID processid );
} mutex_t;


struct mutex* mutex_new();
bool mutex_try_lock( struct mutex* mtx, const PID processid );
bool mutex_try_unlock( struct mutex* mtx, const PID processid );



#ifdef __cplusplus
}
#endif

#endif // _MUTEX_H_