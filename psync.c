// Copyright (C) The OnePointer Authors.
//

#include "psync.h"

#include "ptime.h"
#include "threading.h"

#include <stdlib.h>

struct SyncTime* synctime_new( const struct timespec* current_process_time
                        , const enum PSignals signal_in, struct PThread* thread
                        , callback_sync_finished_f callback_sync_finished ) {
    struct SyncTime* st = (struct SyncTime*) malloc(sizeof(struct SyncTime));

    st->time_current =  timespec_new( current_process_time->tv_sec, current_process_time->tv_nsec );
    st->time_syncpoint = NULL;
    st->time_suspended = NULL;

    st->signal_in = signal_in;
    st->signal_now = NOSIGNAL;
    st->signal_finished = NOSIGNAL;

    st->thread = thread;
    st->sync_wait_suspense = NULL;

    st->cb_sync_finished = callback_sync_finished;

    return st;
}

struct Signal* callback_reentrance(struct semaphore* dsem, const struct SyncTime* st) {
    dsem->lock( dsem, st->thread->threadid );
    return signal_new( sigresume, false, st->thread->threadid
                , NULL, NULL, 0, "Callback reentrance, SIGRESUME." );
}

struct PThread* callback_sync_suspense(const struct timespec* wait_time
                                    , const enum PSignals psig_now, struct PThread* sync_pthr ) {
    pthread_condattr_t* condattr = NULL;
    pthread_condattr_init( condattr );
    pthread_condattr_setpshared( condattr, sync_pthr->pshared );

    pthread_cond_t* cond = NULL;
    pthread_cond_init( cond, condattr );

    pthread_mutex_t* mtx = NULL;
    pthread_mutex_init( mtx, sync_pthr->attr_mutex );

    return ( 0 == pthread_cond_timedwait( cond, mtx, wait_time ) )
        ? sync_pthr : NULL;
}


struct SyncTimeHandler* synctimehandler_new( callback_reentrance_f callback_reentrance
                                , callback_sync_suspense_f callback_sync_suspense ) {
    struct SyncTimeHandler* sth = (struct SyncTimeHandler*) malloc(sizeof(struct SyncTimeHandler));

    sth->cb_reentrance = callback_reentrance;
    sth->cb_sync_suspense = callback_sync_suspense;
    sth->st = NULL;

    return sth;
}



bool syncpgoon_waitsuspensetime( struct SyncTimeHandler* sth, const struct timespec* goon_suspense_time ) {
    if ( sth == NULL || sth->st == NULL ) return false;
    
    if ( sth->st->time_suspended == NULL )
        sth->st->time_suspended = timespec_new( goon_suspense_time->tv_sec, goon_suspense_time->tv_nsec );
    else
        sth->st->time_suspended = ts_plus( sth->st->time_suspended, goon_suspense_time );
    
    struct PThread* thread = sth->cb_sync_suspense( sth->st->time_suspended, SIGSUSPEND, sth->st->thread );
    return thread != NULL;
}


bool syncpgoon_resumetime( struct SyncTimeHandler* sth, const struct timespec* goon_suspense_time ) {
    if ( sth == NULL || sth->st == NULL ) return false;

    sth->st->signal = sth->cb_reentrance( sth->st->sync_wait_suspense->sem, sth->st );
    if ( sth->st->signal != NULL ) // TODO: Signal handling
        sth->st->signal_finished = sth->st->signal->type;
    
    return true;
}



bool syncptime( struct SyncTimeHandler* sth, const enum PSync psync ) {

}

bool syncp( struct SyncTimeHandler* sth ) {
    return synctime( sth, PROCESS_SYNCRONOUS );
}

bool asyncp( struct SyncTimeHandler* sth ) {
    return synctime( sth, PROCESS_ASYNCRONOUS );
}

bool saychildren_then_sync( const enum PSignals psig, struct PThread* parent_thread
                , struct SyncTimeHandler* sth, const enum PSync sync_behaviour ) {

}


struct SyncProcess* syncprocess_new( struct PThread* p1, struct PThread* p2
                            , struct SyncTimeHandler* sph, const enum PSync psync
                            , callback_sync_finished_f callback_sync_finished ) {
    struct SyncProcess* sp = (struct SyncProcess*) malloc(sizeof(struct SyncProcess));

    sp->p1 = synctime_new( NULL, NOSIGNAL, p1, callback_sync_finished );
    sp->p2 = synctime_new( NULL, NOSIGNAL, p2, callback_sync_finished );
    sp->handler = sph;

    sp->psync = psync;
    sp->next = NULL;

    return sp;
}


bool syncprocess( struct SyncTimeHandler* sth, const enum PSync psync ) {

}


bool syncsp( struct SyncTimeHandler* sth ) {

}


bool asyncsp( struct SyncTimeHandler* sth ) {

}

struct SyncProcessHandler* syncprocesshandler_new( callback_sync_f callback_sync, callback_sync_f callback_async ) {
    struct SyncProcessHandler* sph = (struct SyncProcessHandler*)
                        malloc(sizeof(struct SyncProcessHandler));
    
    sph->sp = NULL;
    sph->sp_queue = NULL;

    sph->signals_received = NULL;
    
    sph->cb_sync = callback_sync;
    sph->cb_async = callback_async;

    return sph;
}

bool syncprocesshandler_set_processes( struct SyncProcessHandler* sph, struct PThread* p1, struct PThread* p2 ) {
    if ( sph == NULL || sph->sp == NULL ) return false;
    time_update_since_start();
    struct timespec* ts_sysstart = PTimeToTspec( time_system_start );

    struct ProcessTime* pt_now = systime_now();

    sph->sp->p1 = synctime_new( PTimeToTspec( pt_minus( pt_now, pt_minus( p1->time_pstarted, time_system_start) ) )
                        , SIGSYNC, p1, NULL );
    sph->sp->p2 = synctime_new( PTimeToTspec( pt_minus( pt_now, pt_minus( p2->time_pstarted, time_system_start ) ) )
                        , SIGSYNC, p2, NULL );
    
    return true;
}

bool syncprocesshandler_sync_processes( struct PThread* p1, struct PThread* p2
                            , callback_sync_f callback_sync, callback_sync_f callback_async
                            , const enum PSync psync, callback_sync_finished_f callback_sync_finished ) {
    struct SyncProcessHandler* sph = syncprocesshandler_new( callback_sync, callback_async );
    syncprocesshandler_set_processes( sph, p1, p2 );

    struct SyncTimeHandler* sth = synctimehandler_new( callback_reentrance, callback_sync ); // TODO

    sph->sp = syncprocess_new( p1, p2, sth, psync, callback_sync_finished );
    sph->sp_queue = sph->sp;

    struct PThread* pt1 = p1;
    struct PThread* pt2 = p2;

    // sph->sp->p1->time_current = 
}

bool syncprocesshandler_sync_children( struct PThread* parent_process ) {

}

bool syncprocesshandler_sync_with_parents( struct PThread* process ) {

}
