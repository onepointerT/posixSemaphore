// Copyright (C) 2026 The OnePointer Authors.
//

#ifnef _PTIME_H_
#define _PTIME_H_


#ifdef __cplusplus
extern "C" {
#endif

#include "semaphore.h"
#include "systime.h"

#include <stdbool.h>


typedef struct ProcessTime {
    unsigned short hour;
    unsigned short minute;
    unsigned short day;
    struct timespec* tspec;

    struct timespec* elapsed;
    tick_t elapsed_ticks;
    deltatime_t elapsed_time;

    tick_t tps; // Ticks per second for this computer

    struct timespec* (*update_elapsed)( struct ProcessTime* pt );
} process_time_t;

struct timespec* timespec_new( const time_t seconds, const long nanoseconds );
struct ProcessTime* processtime_new( unsigned short hour, unsigned short minute
                            , unsigned short day, time_t seconds, long nseconds );


struct ProcessTime* systime_started();
struct ProcessTime* systime_now();
struct ProcessTime* systime_since_start();

constexpr void time_init();
constexpr inline struct ProcessTime* time_system_start;
constexpr inline struct ProcessTime* time_since_start;
constexpr void time_update_since_start();

tick_t seconds_to_ticks( const deltatime_t dt );
struct timespec* elapsed_since( struct ProcessTime* pt );

struct ProcessTime* tsToPTime( const struct timespec* tspec );
deltatime_t tsToDeltaTime( const struct timespec* tspec );
tick_t tsToTicks( const struct timespec* tspec );
struct timespec* PTimeToTspec( const struct ProcessTime* pt );
struct timespec* ticksToTs( const tick_t ticks );
struct timespec* deltatimeToTs( const deltatime_t deltatime );
struct ProcessTime* dtToPTime( const deltatime_t deltatime );

struct ProcessTime* pt_copy( const struct ProcessTime* pt );
struct ProcessTime* pt_minus( const struct ProcessTime* pt1, const struct ProcessTime* pt2 );
struct ProcessTime* pt_plus( const struct ProcessTime* pt1, const struct ProcessTime* pt2 );
struct ProcessTime* pt_times( const struct ProcessTime* pt1, const struct ProcessTime* pt2 );
struct ProcessTime* pt_divide( const struct ProcessTime* pt1, const struct ProcessTime* pt2 );
struct ProcessTime* pt_modulo( const struct ProcessTime* pt1, const struct ProcessTime* pt2 );

struct timespec* ts_copy( const struct timespec* ts );
struct timespec* ts_minus( const struct timespec* ts1, const struct timespec* ts2 );
struct timespec* ts_plus( const struct timespec* ts1, const struct timespec* ts2 );
struct timespec* ts_times( const struct timespec* ts1, const struct timespec* ts2 );
struct timespec* ts_divide( const struct timespec* ts1, const struct timespec* ts2 );
struct timespec* ts_modulo( const struct timespec* ts1, const struct timespec* ts2 );

// deltatime_t 

const unsigned int sizeofnum( const long num );
const unsigned int sizeofnumd_behind_comma( const double num );
const unsigned int sizeofnumd_before_comma( const double num );
const double sizeofnum_factor( const unsigned int length_of_num, const bool pre_comma );

#ifdef __cplusplus
}
#endif


#endif // _PTIME_H_