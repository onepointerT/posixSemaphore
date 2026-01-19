// Copyright (C) 2026 The OnePointer Authors.
//

#ifndef _THREADINGPOOL_H_
#define _THREADINGPOOL_H_


#ifdef __cplusplus
extern "C" {
#endif

#include "semaphore_type.h"
#include "threading.h"

#include <pthread.h>

#include <stdint.h>


struct ThreadingTrajection;


typedef struct ThreadingQueue {
    thread_t** joins;
    struct timespec** join_timeouts;
    size_t join_size;
    size_t join_max_size;
    struct ThreadingTrajection* join_epimorph_traj;

    thread_t** waitings;
    struct timespec** waiting_timeouts;
    size_t waiting_size;
    size_t waiting_max_size;
    struct ThreadingTrajection* waiting_endomorph_traj;

    struct ThreadingTrajection* epimorph_traj_startone;
    struct ThreadingTrajection* endomorph_traj_threading;

    thread_t* first;
    thread_t* last;
    thread_t** que;
} threading_queue_t;



typedef struct ThreadingNode {
    thread_t* thrd;
    semaphore_t* sem;

    struct ThreadingNode* parent;
    struct ThreadingNode** chlds;
    struct ThreadingNode** leafs;

    struct ThreadingTrajection* traj_parent_here;
    struct ThreadingTrajection* traj_here_parent;
    struct ThreadingTrajection* traj_spawn_fork;
    struct ThreadingTrajection* traj_to_children;

    struct ThreadingQueue* queue;
} threading_node_t;

typedef struct ThreadingPool {
    thread_t* root;
    thread_t** threads;
    size_t pool_size;

    threading_node_t** nodes;
    threading_node_t* node_root;
    threading_node_t* node_current;

    thread_t* first;

} threading_pool_t;



#ifdef __cplusplus
}
#endif



#endif // _THREADINGPOOL_H_