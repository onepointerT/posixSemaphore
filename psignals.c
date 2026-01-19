// Copyright (C) 2026 The OnePointer Authors.
//

#include "psignals.h"

#include <stdlib.h>


struct PID_Array* pid_array_new( const unsigned long process_id, struct PID_Array* first ) {
    struct PID_Array* pida = (struct PID_Array*) malloc(sizeof(struct PID_Array));

    pida->process_id = process_id;
    pida->next = NULL;
    pida->prev = NULL;
    if ( first == NULL ) pida->first = pida;
    else pida->first = first;

    return pida;
}


struct PID_Array* pid_array_add( const unsigned long process_id, struct PID_Array* pida ) {
    struct PID_Array* next = pid_array_new( process_id, pida );
    struct PID_Array* last = pida;
    while ( last->next != NULL ) { last = last->next; }
    last->next = next;
    next->prev = last;
    return next;
}


bool pid_array_is_first( const struct PID_Array* pida ) {
    return pida != NULL & pida == pida->first;
}

struct Signal* signal_new( const PSignals psig, const bool locked
                    , const unsigned long receiver_thread_id
                    , struct ParamPack* param_pack, struct PID_Array* receivers
                    , const size_t results_size, const char* msg ) {
    struct Signal* sig = (struct Signal*) malloc(sizeof(struct Signal));

    sig->type = psig;
    sig->locked = locked;
    sig->processed = false;

    sig->msg = msg;
    sig->results = (char**) malloc(sizeof(char*)*results_size);
    for ( unsigned int r = 0; r < results_size; r++ ) {
        sig->results[r] = NULL;
    }

    sig->dsem = semaphore_type_new( NULL, PTHREAD_PROCESS_SHARED );
    sig->param = param_pack;

    sig->receiver_thread_id = receiver_thread_id;

    sig->receiver_threads = receivers;
    sig->processed_by_ids = NULL;
    sig->processed_completly = false;

    return sig;
}


bool signal_add_receiver( struct Signal* sig, const unsigned long pid_recv ) {
    if ( sig == NULL ) return false;
    else if ( sig->processed_by_ids == NULL ) sig->processed_by_ids = pid_array_new( pid_recv, NULL );
    else pid_array_add( pid_recv, sig->processed_by_ids );
    return true;
}

struct SignalQueue* signalqueue_new( struct Signal* first_elem, struct SignalQueue* first_elem ) {
    struct SignalQueue* sq = (struct SignalQueue*) malloc(sizeof(struct SignalQueue));

    sq->sig = first_elem;
    if ( first_elem == NULL ) sq->first = sq;
    else sq->first = first_elem;
    sq->next = NULL;
    sq->prev = NULL;

    return sq;
}


struct SignalQueue* signalqueue_add( struct Signal* new_elem, struct SignalQueue* sigq_first ) {
    struct SignalQueue* sq_new = signalqueue_new( new_elem, sigq_first );

    struct SignalQueue* sq_last = sigq_first;
    while ( sq_last->next != NULL ) { sq_last = sq_last->next; }
    sq_new->prev = sq_last;
    sq_last->next = sq_new;

    return sq_new;
}