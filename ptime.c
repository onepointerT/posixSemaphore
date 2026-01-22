// Copyright (C) 2026 The OnePointer Authors.
//

#include "ptime.h"

#include <stdlib.h>
#include <string.h>


struct timespec* timespec_new( const time_t seconds, const long nanoseconds ) {
    struct timespec* tsp = (struct timespec*) malloc(sizeof(struct timespec));

    tsp->tv_sec = seconds;
    tsp->tv_nsec = nanoseconds;

    return tsp;
}

struct ProcessTime* processtime_new( unsigned short hour, unsigned short minute
                            , unsigned short day, time_t seconds, long nseconds ) {
    struct ProcessTime* pt = (struct ProcessTime*) malloc(sizeof(struct ProcessTime));

    pt->hour = hour;
    pt->minute = minute;
    pt->day = day;
    pt->tspec = timespec_new( seconds, nseconds );

    pt->elapsed = NULL;
    pt->elapsed_ticks = 0;
    pt->elapsed_time = 0.0;

    pt->tps = timer_ticks_per_second();

    pt->update_elapsed = &elapsed_since;

    return pt;
}



struct ProcessTime* systime_started() {
    const deltatime_t tstart = timer_elapsed( 0 );
    return dtToPTime( tstart );
}


struct ProcessTime* systime_now(){
    const tick_t tnow = timer_current();
    return tsToPTime( tickToTs(tnow) );
}


struct ProcessTime* systime_since_start(){
    const struct ProcessTime* pt_now = systime_now();
    return pt_minus( pt_now, time_system_start );
}


void time_init() {
    timer_lib_initialize();
    time_system_start = systime_started();
    time_since_start = systime_since_start();
}

void time_update_since_start(){
    time_since_start = systime_since_start();
}


tick_t seconds_to_ticks( const deltatime_t dt ) {
    if ( time_system_start == NULL ) time_init();
    if ( time_since_start == NULL ) time_update_since_start();

    tick_t tps = ( timer_ticks_per_second() + time_system_start->tps + time_since_start->tps ) / 3;

    return (tick_t) ((((tps * dt) + 0.5 * (dt / tps)) / ( 2 )) + (0.66 * ((dt) / (tps)))); // tps*dt als Normierung
}


struct timespec* elapsed_since( struct ProcessTime* pt ) {
    struct ProcessTime* pt_now = systime_now();

    struct ProcessTime* pt_difference = pt_minus( pt_now, pt );

    pt->elapsed = PTimeToTspec( pt_difference );
    pt->elapsed_time = tsToDeltaTime( pt->elapsed );
    pt->elapsed_ticks = seconds_to_ticks( pt->elapsed_time );

    return pt->elapsed;
}


struct ProcessTime* tsToPTime( const struct timespec* tspec ){
    deltatime_t dt = tsToDeltaTime( tspec );
    struct timespec* ts = ts_copy( tspec );
    
    short days = dt % (24*60*60);
    time_t days_in_seconds = days * 24*60*60;
    ts = ts_minus( ts, timespec_new( days_in_seconds, 0 ) );

    dt -= days_in_seconds;
    short hours = dt % (60*60);
    time_t hours_in_seconds = hours * 60*60;
    ts = ts_minus( ts, timespec_new( hours_in_seconds, 0 ) );

    dt -= hours_in_seconds;
    short minutes = dt % 60;
    time_t minutes_in_seconds = minutes * 60;
    ts = ts_minus( ts, timespec_new( minutes_in_seconds, 0 ) );
    
    dt -= minutes_in_seconds;
    hours += minutes % 60;
    minutes -= ( minutes % 60 ) * 60;
    days += hours % 24;
    hours -= (hours % 24) * 24;
    time_t seconds = dt;

    return processtime_new( hours, minutes, days, seconds, 0 );
}


deltatime_t tsToDeltaTime( const struct timespec* tspec ){
    tick_t tt = tsToTicks( tspec );

    return timer_ticks_to_seconds( tt );
}


tick_t tsToTicks( const struct timespec* tspec ){
    const unsigned int sizeof_nsec = sizeofnum( tspec->tv_nsec );

    deltatime_t dt = 0.0 + (double) tspec->tv_sec;
    dt += 0.0 + (double) tspec->tv_nsec * sizeofnum_factor( tspec->tv_nsec );

    return seconds_to_ticks( dt );
}


struct timespec* PTimeToTspec( const struct ProcessTime* pt ) {
    time_t seconds = (pt->day * 24*60*60) + (pt->hour * 60*60) + (pt->minute * 60);
    return timespec_new( seconds, 0 );
}


struct timespec* ticksToTs( const tick_t ticks ){
    deltatime_t dt = timer_ticks_to_seconds( ticks );

    return deltatimeToTs( dt );
}


struct timespec* deltatimeToTs( const deltatime_t deltatime ){
    const int sec = (int) deltatime;
    const int nsec = (int) deltatime - sec;

    return timespec_new( sec, nsec );
}


struct ProcessTime* dtToPTime( const deltatime_t deltatime ){
    struct timespec* ts = deltatimeToTs( deltatime );

    return tsToPTime( ts );
}

struct ProcessTime* pt_minus( const struct ProcessTime* pt1, const struct ProcessTime* pt2 ){
    const struct timespec* ts1 = PTimeToTspec( pt1 );
    const struct timespec* ts2 = PTimeToTspec( pt2 );

    const time_t tminus = ts1->tv_sec - ts2->tv_sec;

    return tsToPTime( timespec_new( tminus, 0 ) );
}


struct ProcessTime* pt_plus( const struct ProcessTime* pt1, const struct ProcessTime* pt2 ){
    const struct timespec* ts1 = PTimeToTspec( pt1 );
    const struct timespec* ts2 = PTimeToTspec( pt2 );

    const time_t tplus = ts1->tv_sec + ts2->tv_sec;

    return tsToPTime( timespec_new( tplus, 0 ) );
}


struct ProcessTime* pt_times( const struct ProcessTime* pt1, const struct ProcessTime* pt2 ){
    const struct timespec* ts1 = PTimeToTspec( pt1 );
    const struct timespec* ts2 = PTimeToTspec( pt2 );

    const time_t ttimes = ts1->tv_sec * ts2->tv_sec;

    return tsToPTime( timespec_new( ttimes, 0 ) );
}


struct ProcessTime* pt_divide( const struct ProcessTime* pt1, const struct ProcessTime* pt2 ){
    const struct timespec* ts1 = PTimeToTspec( pt1 );
    const struct timespec* ts2 = PTimeToTspec( pt2 );

    const time_t tdivide = ts1->tv_sec / ts2->tv_sec;

    return tsToPTime( timespec_new( tdivide, 0 ) );
}


struct ProcessTime* pt_modulo( const struct ProcessTime* pt1, const struct ProcessTime* pt2 ){
    const struct timespec* ts1 = PTimeToTspec( pt1 );
    const struct timespec* ts2 = PTimeToTspec( pt2 );

    const time_t tmodulo = ts1->tv_sec % ts2->tv_sec;

    return tsToPTime( timespec_new( tmodulo, 0 ) );
}



struct timespec* ts_minus( const struct timespec* ts1, const struct timespec* ts2 ){
    return timespec_new( ts1->tv_sec - ts2->tv_sec, ts1->tv_nsec - ts2->tv_nsec );
}


struct timespec* ts_plus( const struct timespec* ts1, const struct timespec* ts2 ){
    return timespec_new( ts1->tv_sec + ts2->tv_sec, ts1->tv_nsec + ts2->tv_nsec );
}


struct timespec* ts_times( const struct timespec* ts1, const struct timespec* ts2 ){
    return timespec_new( ts1->tv_sec * ts2->tv_sec, ts1->tv_nsec * ts2->tv_nsec );
}


struct timespec* ts_divide( const struct timespec* ts1, const struct timespec* ts2 ){
    return timespec_new( ts1->tv_sec / ts2->tv_sec, ts1->tv_nsec / ts2->tv_nsec );
}


struct timespec* ts_modulo( const struct timespec* ts1, const struct timespec* ts2 ){
    return timespec_new( ts1->tv_sec % ts2->tv_sec, ts1->tv_nsec % ts2->tv_nsec );
}


const unsigned int sizeofnum( const long num ) {
    char strbuf[35];
    const int sizeofl = sprintf_s( strbuf, 35, "%ld", num );
    return sizeofl > -1 ? sizeofl : 0;
}


const unsigned int sizeofnumd_behind_comma( const double num ) {
    const long num_before_comma = num % 1.0;
    const double num_behind_comma_only = num - num_before_comma;
    const long num_behind_comma = num_behind_comma_only % 0.1;

    return sizeofnum( num_behind_comma );
}


const unsigned int sizeofnumd_before_comma( const double num ) {
    const long num_before_comma = num % 1.0;

    return sizeofnum( num_before_comma );
}

const double sizeofnum_factor( const unsigned int length_of_num, const bool pre_comma ) {
    if ( pre_comma ) return 0.0 + (double) length_of_num;
    
    double factor = 0.0;
    for ( int lon = length_of_num; lon >= 0; lon-- ) { factor *= 0.1; }
    return factor;
}