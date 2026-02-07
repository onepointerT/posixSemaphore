// Copyright (C) 2026 The OnePointer Authors.
//

#include "scheduler.h"

#include <stddef.h>
#include <stdlib.h>


struct scheduler* scheduler_new() {
    struct scheduler* sched = (struct scheduler*) malloc(sizeof(struct scheduler));

    sched->lock = NULL;
    sched->mtx = (mutex_t**) malloc(sizeof(mutex_t*)*SCHEDULE_NUMBER_MUTEXES);
    sched->mtx_names = (char**) malloc(sizeof(char*)*SCHEDULE_NUMBER_MUTEXES);

    sched->mem_shared = memory_new( PRIVATE );

    return sched;
}