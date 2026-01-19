// Copyright (C) 2026 The OnePointer Authors.
//

#ifndef _PSIGNALS_H_
#define _PSIGNALS_H_

#ifdef __cplusplus
extern "C" {
#endif



#include <stdbool.h>

#include "mutex.h"
#include "parampack.h"
#include "semaphore_type.h"


/*
 *	+--------------------+------------------+
 *	|  POSIX signal      |  default action  |
 *	+--------------------+------------------+
 *	|  SIGHUP            |  terminate	|
 *	|  SIGINT            |	terminate	|
 *	|  SIGQUIT           |	coredump 	|
 *	|  SIGILL            |	coredump 	|
 *	|  SIGTRAP           |	coredump 	|
 *	|  SIGABRT/SIGIOT    |	coredump 	|
 *	|  SIGBUS            |	coredump 	|
 *	|  SIGFPE            |	coredump 	|
 *	|  SIGKILL           |	terminate(+)	|
 *	|  SIGUSR1           |	terminate	|
 *	|  SIGSEGV           |	coredump 	|
 *	|  SIGUSR2           |	terminate	|
 *	|  SIGPIPE           |	terminate	|
 *	|  SIGALRM           |	terminate	|
 *	|  SIGTERM           |	terminate	|
 *	|  SIGCHLD           |	ignore   	|
 *	|  SIGCONT           |	ignore(*)	|
 *	|  SIGSTOP           |	stop(*)(+)  	|
 *	|  SIGTSTP           |	stop(*)  	|
 *	|  SIGTTIN           |	stop(*)  	|
 *	|  SIGTTOU           |	stop(*)  	|
 *	|  SIGURG            |	ignore   	|
 *	|  SIGXCPU           |	coredump 	|
 *	|  SIGXFSZ           |	coredump 	|
 *	|  SIGVTALRM         |	terminate	|
 *	|  SIGPROF           |	terminate	|
 *	|  SIGPOLL/SIGIO     |	terminate	|
 *	|  SIGSYS/SIGUNUSED  |	coredump 	|
 *	|  SIGSTKFLT         |	terminate	|
 *	|  SIGWINCH          |	ignore   	|
 *	|  SIGPWR            |	terminate	|
 *	|  SIGRTMIN-SIGRTMAX |	terminate       |
 *	+--------------------+------------------+
 *	|  non-POSIX signal  |  default action  |
 *	+--------------------+------------------+
 *	|  SIGEMT            |  coredump	|
 *	+--------------------+------------------+
 */
typedef enum P_SIGNALS
{
     SIGHUP            //   |   terminate	|
   , SIGINT            //   |    terminate	|
   , SIGQUIT           //   |    coredump 	|
   , SIGILL            //   |    coredump 	|
   , SIGTRAP           //   |    coredump 	|
   , SIGABRT           //   |    coredump 	|
   , SIGIOT            //   |    coredump 	|
   , SIGBUS            //   |    coredump 	|
   , SIGFPE            //   |    coredump 	|
   , SIGKILL           //   |    terminate(+)	|
   , SIGUSR1           //   |    terminate	|
   , SIGSEGV           //   |    coredump 	|
   , SIGUSR2           //   |    terminate	|
   , SIGPIPE           //   |    terminate	|
   , SIGALRM           //   |    terminate	|
   , SIGTERM           //   |    terminate	|
   , SIGCHLD           //   |    ignore   	|
   , SIGCONT           //   |    ignore(*)	|
   , SIGSTOP           //   |    stop(*)(+)  	|
   , SIGTSTP           //   |    stop(*)  	|
   , SIGTTIN           //   |    stop(*)  	|
   , SIGTTOU           //   |    stop(*)  	|
   , SIGURG            //   |    ignore   	|
   , SIGXCPU           //   |    coredump 	|
   , SIGXFSZ           //   |    coredump 	|
   , SIGVTALRM         //   |    terminate	|
   , SIGPROF           //   |    terminate	|
   , SIGPOLL           //   |    terminate	|
   , SIGIO             //   |    terminate	|
   , SIGSYS            //   |    coredump 	|
   , SIGUNUSED         //   |    coredump 	|
   , SIGSTKFLT         //   |    terminate	|
   , SIGWINCH          //   |    ignore   	|
   , SIGPWR            //   |    terminate	|
   , SIGRTMIN          //   |    terminate  |
   , SIGRTMAX          //   |    terminate  |
}
PSignals;

typedef struct PID_Array {
    unsigned long process_id;
    
    struct PID_Array* next;
    struct PID_Array* prev;

    struct PID_Array* first;
} pid_array_t;


struct PID_Array* pid_array_new( const unsigned long process_id, struct PID_Array* first );
struct PID_Array* pid_array_add( const unsigned long process_id, struct PID_Array* pida );
bool pid_array_is_first( const struct PID_Array* pida );


typedef struct Signal {
    PSignals type;
    bool locked;
    bool processed;

    char* msg;
    char** results;
    
    struct semaphore* dsem;
    struct ParamPack* param;

    unsigned long receiver_thread_id;

    struct PID_Array* receiver_threads;
    struct PID_Array* processed_by_ids;
    bool processed_completly;
} signal_t;


struct Signal* signal_new( const PSignals psig, const bool locked
    , const unsigned long receiver_thread_id
    , struct ParamPack* param_pack, struct PID_Array* receivers
    , const size_t results_size, const char* msg );
bool signal_add_receiver( struct Signal* sig, const unsigned long pid_recv );


typedef struct SignalQueue {
    struct Signal* sig;

    struct SignalQueue* next;
    struct SignalQueue* prev;

    struct SignalQueue* first;
} signal_queue_t;

struct SignalQueue* signalqueue_new( struct Signal* first_elem, struct SignalQueue* first_elem );
struct SignalQueue* signalqueue_add( struct Signal* new_elem, struct SignalQueue* sigq_first );


#ifdef __cplusplus
}
#endif

#endif // _PSIGNALS_H_