// Copyright (C) The OnePointer Authors.
//

#ifndef _PSYNC_H_
#define _PSYNC_H_


#ifdef __cplusplus
extern "C" {
#endif

#include "psignals.h"
#include "ptime.h"
#include "semaphore.h"
#include "threading.h"

#include <stdbool.h>
#include <stdint.h>

// Forward declaration
struct SyncTime;


typedef struct Signal* (*callback_reentrance_f)(struct semaphore* dsem, const struct SyncTime* st);
typedef struct PThread* (*callback_sync_suspense_f)(const struct timespec* wait_time
                                    , const enum PSignals psig_now, struct PThread* sync_pthr);
typedef int (*callback_sync_finished_f)(const enum PSignals psig, const struct SyncTime* synctm);

typedef struct SyncTime {
    struct ProcessTime* time_current;
    struct ProcessTime* time_syncpoint;
    struct timespec* time_suspended;

    enum PSignals signal_in;
    enum PSignals signal_now;
    enum PSignals signal_finished;
    struct Signal* signal;

    struct PThread* thread;
    struct PThread* sync_wait_suspense;

    callback_sync_finished_f cb_sync_finished;
} sync_time_t;


struct SyncTime* synctime_new( const struct timespec* current_process_time
                        , const enum PSignals signal_in, struct PThread* thread
                        , callback_sync_finished_f callback_sync_finished );

struct Signal* callback_reentrance(struct semaphore* dsem, const struct SyncTime* st);
struct PThread* callback_sync_suspense(const struct timespec* wait_time
                                    , const enum PSignals psig_now, struct PThread* sync_pthr);
extern int callback_sync_finished(const enum PSignals psig, const struct SyncTime* synctm);


typedef struct SyncTimeHandler {
    struct SyncTime* st;

    callback_reentrance_f cb_reentrance;
    callback_sync_suspense_f cb_sync_suspense;
} sync_time_handler_t;


typedef enum
{
    PROCESS_ASYNCRONOUS = 0,
    PROCESS_SYNCRONOUS = 1
}
PSync;

struct SyncTimeHandler* synctimehandler_new( callback_reentrance_f callback_reentrance
                                , callback_sync_suspense_f callback_sync_suspense );
bool syncpgoon_waitsuspensetime( struct SyncTimeHandler* sth, const struct timespec* goon_suspense_time );
bool syncpgoon_resumetime( struct SyncTimeHandler* sth, const struct timespec* goon_suspense_time );
bool syncptime( struct SyncTimeHandler* sth, const enum PSync psync );
bool syncp( struct SyncTimeHandler* sth );
bool asyncp( struct SyncTimeHandler* sth );
bool saychildren_then_sync( const enum PSignals psig, struct PThread* parent_thread
                , struct SyncTimeHandler* sth, const enum PSync sync_behaviour );



typedef enum PSignals (*callback_sync_f)(const struct SyncProcess* sp);


typedef struct SyncProcess {
    struct SyncTime* p1;
    struct SyncTime* p2;

    struct SyncTimeHandler* handler;

    enum PSync psync;

    struct SyncProcess* next;
} sync_process_t;


struct SyncProcess* syncprocess_new( struct PThread* p1, struct PThread* p2
                            , struct SyncTimeHandler* sph, const enum PSync psync
                            , callback_sync_finished_f callback_sync_finished );
bool syncprocess( struct SyncTimeHandler* sth, const enum PSync psync );
bool syncsp( struct SyncTimeHandler* sth );
bool asyncsp( struct SyncTimeHandler* sth );


typedef struct SyncProcessHandler {
    struct SyncProcess* sp;
    struct SyncProcess* sp_queue;
    
    struct SignalQueue* signals_received;

    callback_sync_f cb_sync;
    callback_sync_f cb_async;
} sync_process_handler_t;

struct SyncProcessHandler* syncprocesshandler_new( callback_sync_f callback_sync, callback_sync_f callback_async );
bool syncprocesshandler_set_processes( struct SyncProcessHandler* sph, struct PThread* p1, struct PThread* p2 );
bool syncprocesshandler_sync_processes( struct PThread* p1, struct PThread* p2
                        , callback_sync_f callback_sync, callback_sync_f callback_async, const enum PSync psync
                        , callback_sync_finished_f callback_sync_finished );
bool syncprocesshandler_sync_children( struct PThread* parent_process );
bool syncprocesshandler_sync_with_parents( struct PThread* process );
bool syncprocesshandler_sync_with_parents( struct PThread* process );


#ifdef __cplusplus
}
#endif


#endif // _PSYNC_H_