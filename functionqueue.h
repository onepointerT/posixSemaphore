// Copyright (C) 2026 The OnePointer Authors.
//

#ifndef _FUNCTIONQUEUE_H_
#define _FUNCTIONQUEUE_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "psignals.h"
#include "resultenv.h"
#include "semaphore.h"
#include "truth.h"


typedef void* (*thread_f)(void*);
typedef struct ResultEnvironment* (*process_function_f)(void** data, struct Environment* parameters, thread_f tfunc);
typedef thread_f (*thread_init_f)(void** data, struct Environment* parameters);

typedef struct NamedFunction {
    char* funcname;
    process_function_f function;
    thread_f function_thread;
    thread_init_f function_thread_pre;

    double priority;
    struct timespec* timeout;

    struct TruthEquivation* condition_start;
    struct TruthEquivation* condition_resume_parent;
} named_function_t;

struct NamedFunction* namedfunction_init( const char* name, process_function_f pf
                            , thread_f tf, thread_init_f tif, double priority
                            , const struct timespec* tout );


typedef struct FunctionMap {
    char** keys;
    process_function_f* f;
    struct TruthEquivation** condition_start_function;
    struct TruthEquivation** condition_resume_parent_function;
} function_map_t;


struct FunctionMap* function_map;


typedef struct FunctionQueueNode {
    struct NamedFunction* nf;
    
    struct SignalQueue* sq;

    struct FunctionQueueNode* next;
    struct FunctionQueueNode* prev;
} function_queue_node_t;


typedef enum F_QUEUE_TYPE
{
    FIFO,
    LIFO,
    FILO,
    SCHEDULED,
    PRIORITY,
    GOTO
}
FQueueType;


typedef struct FunctionQueue {
    struct FunctionQueueNode* current;

    struct FunctionQueueNode* front;
    struct FunctionQueueNode* back;

    enum FQueueType type;
} function_queue_t;


#ifdef __cplusplus
}
#endif


#endif // _FUNCTIONQUEUE_H_