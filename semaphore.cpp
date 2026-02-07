// Copyright (C) 2025 The OnePointer Authors
//

#include "semaphore.hpp"

extern "C" {

}

#include <cstdlib>
#include <string>

namespace psem {


unsigned int semaphore_t::sid = 0;


semaphore_t::semaphore_t( void* data, const char* name  )
    :   arch_sem_t{ .handle = data }
    ,   sem( sem_open( name + ++this->sid, _O_CREAT, _MODE_T_, this->sid ) )
    ,   mtx( mutex_new() )
{}


bool semaphore_t::try_lock( const PID process_id ) {
    return this->mtx->try_lock( this->mtx, process_id );
}

bool semaphore_t::try_unlock( const PID process_id ) {
    return this->mtx->try_unlock( this->mtx, process_id );
}

DHANDLE semaphore_t::data() const {
    return this->handle;
}

const PID semaphore_t::process_id() const {
    return this->mtx->process_id;
}

const bool semaphore_t::locked() const {
    return this->mtx->locked;
}

const std::string semaphore_t::name() const {
    return this->sname;
}


struct timespec* DataSemaphore::make_timespec( const time_t seconds, const long nanoseconds ) {
    struct timespec* tsp = (struct timespec*) malloc(sizeof(struct timespec));

    tsp->tv_sec = seconds;
    tsp->tv_nsec = nanoseconds;

    return tsp;
}

bool DataSemaphore::is_free() const {
    return ! this->sem.locked();
}

bool DataSemaphore::is_lockable() const {
    return ! this->dlocked() && this->is_free();
}

bool DataSemaphore::is_available() const {
    return ! this->dsem.locked();
}


DataSemaphore::ProcessIdentifier::ProcessIdentifier( const PID process_id, const std::string aquiring_reason )
    :   process_identifier_t{ process_id, aquiring_reason }
    ,   callback( nullptr )
    ,   notifier( nullptr )
    ,   paused( false )
    ,   wait_time( nullptr )
{}


DataSemaphore::ProcessIdentifier::ProcessIdentifier( const process_identifier_t pident )
    :   process_identifier_t{ pident.first, pident.second }
    ,   callback( nullptr )
    ,   notifier( nullptr )
    ,   paused( false )
    ,   wait_time( nullptr )
{}


bool DataSemaphore::ProcessIdentifier::empty() const {
    return this->first == 0 || this->second.length() == 0;
}

DataSemaphore::ProcessIdentifier::operator PID() const { return this->first; }
DataSemaphore::ProcessIdentifier::operator std::string() const { return this->second; }

PID DataSemaphore::ProcessIdentifier::call( BS::binary_semaphore* lock, mutex_t* mt, sem_t* sema, void* data ) const {
    if ( this->callback == nullptr ) return 0;
    return this->callback( lock, mt, sema, data );
}

PID DataSemaphore::ProcessIdentifier::notify( BS::binary_semaphore* lock, mutex_t* mt, sem_t* sema, void* data ) const {
    if ( this->notifier == nullptr ) return 0;
    return this->notifier( lock, mt, sema, data );
}

DataSemaphore::process_identifier_t DataSemaphore::find_waiter( const PID process_id ) const {
    for ( typename process_map_t::const_iterator cit : 
                { this->waitings.cbegin(), this->waitings.cend() }
    ) {
        if ( cit->first.first == process_id ) return cit->first;
    }
    return { this->pid, this->aquiring_reason };
}

DataSemaphore::time_aquiring_t DataSemaphore::find_waiting_aquirement( const process_identifier_t pident ) {
    for ( typename process_map_t::iterator it : 
                { this->waitings.begin(), this->waitings.end() }
    ) {
        if ( it->first.first == pident.first && it->first.second == pident.second ) {
            return it->second;
        }
    }
    return { DataSemaphore::make_timespec(0, -1)
                    , { nullptr, pident.first }
    };
}

DataSemaphore::process_queue_t
            DataSemaphore::find_waiter_by_aquirementReason( const std::string aquirement_reason ) const {
    process_queue_t* pqueue = new process_queue_t();

    for ( typename process_map_t::const_iterator cit :
            { this->waitings.cbegin(), this->waitings.cend() }
    ) {
        if ( cit->first.second == aquirement_reason ) {
            pqueue->push( *cit );
        }
    }

    return *pqueue;
}

DataSemaphore::callback_f DataSemaphore::find_fulfillment( const PID process_id ) const {
    for ( typename fulfillment_map_t::const_iterator cit : 
                { this->fulfillment_callbacks.cbegin(), this->fulfillment_callbacks.cend() }
    ) {
        if ( cit->first.first == process_id ) return cit->second->callback;
    }
    return nullptr;
}

DataSemaphore::DataSemaphore( void* data, const char* name )
    :   BS::binary_semaphore(0)
    ,   dsem( *new semaphore_t( data, name ) )
    ,   pid_paused( 0 )
    ,   aquiring_reason_paused( "" )
    ,   sem( *new semaphore_t( NULL, name ) )
    ,   pid( 0 )
    ,   aquiring_reason( "" )
    ,   wait_lock( 0 )
    ,   waitings()
    ,   fulfillment_callbacks()
{}

bool DataSemaphore::lock( const PID process_id, const enum ProcessAbility pshared ) {
    if ( ! this->is_free() || ! this->is_lockable() ) return false;
    else if ( ! this->sem.try_lock(process_id) ) return false;
    else this->pid = process_id;
    if ( pid == 0 || ! this->dsem.try_lock(process_id) || this->dsem.process_id() == 0 ) return false;
    else if ( pshared == ProcessAbility::PROCESS_SHARED ) {
        this->sem.sem = sem_open( this->dsem.process_id() + std::to_string(this->pid).c_str()
                                , _O_CREAT, _MODE_T_, this->pid );
        sem_init( this->sem.sem, 0, 0 );
    } else return false;
    
    this->dshared = pshared;
    this->aquiring_reason = std::to_string(this->pid);
    this->sem.mtx->process_id = this->pid;

    return true;
}

bool DataSemaphore::unlock( const PID process_id ) {
    if ( ! this->sem.try_lock( process_id ) && pid == process_id ) {
        if ( ! this->dsem.try_unlock( process_id ) ) return false;
        this->pid = 0;
        this->sem.mtx->process_id = 0;

        return this->sem.try_unlock( process_id );
    } return false;
}

sem_t* DataSemaphore::aquire_semaphore( const PID process_id, const std::string aquiring_reason
            , callback_f callback, struct timespec* waiting_time ) {
   process_identifier_t pident = { process_id, aquiring_reason };
   aquiring_lock_t al = { callback, process_id };
   time_aquiring_t ta = { waiting_time, al };

   if ( callback != nullptr ) {
        this->waitings.emplace( pident, ta );
   }
    


   sem_t* sm = sem_open(  pident.second.c_str(), O_CREAT, _MODE_T_, 1 );
   if ( ! this->sem.try_lock( pident.first ) ) return nullptr;
   this->dsem.mtx->locked = true;
   return sm;
}


bool DataSemaphore::unaquire_semaphore( const PID process_id, const std::string aquiring_reason ) {
    try {
        process_identifier_t pident = {process_id, aquiring_reason};
        sem_t* sm = sem_open( pident.second.c_str(), O_EXCL, _MODE_T_, 0 );
        if ( sm != nullptr ) {
            sem_close( sm );
            sem_unlink( pident.second.c_str() );
            this->sem.mtx.unlock();
            return true;
        } return true;
    } catch ( std::out_of_range& oor ) { return false; }
}


DataSemaphore::result_t DataSemaphore::wait_for( const time_t seconds, const long nanoseconds, const PID process_id
                                               , callback_f callback, callback_f fulfillment_callback, bool callit ) {
    struct timespec* tspec = DataSemaphore::make_timespec( seconds, nanoseconds );
    process_identifier_t pident{ process_id, "priority_now" };
    time_aquiring_t ta{ tspec, { callback, process_id } };
    this->waitings.emplace( pident, ta );
    this->fulfillment_callbacks.emplace( process_id, fulfillment_callback );

    if ( callit ) {
        if ( this->stop() ) {
            sem_t* sm = this->aquire_semaphore( process_id, "priority_now", callback, tspec );
            if ( sm == nullptr ) return { process_id, nullptr };
            if ( callback != nullptr ) {
                if ( callback( this, this->dsem.mtx, sm, this->sem.handle ) == process_id ) {
                    if ( fulfillment_callback != nullptr ) {
                        if ( fulfillment_callback( this, this->dsem.mtx, sm, this->sem.handle ) != process_id )
                            return { process_id, nullptr };
                    }
                }
                else return { process_id, nullptr };
            }
        }
    }

    return { process_id, this->sem.handle };
}


bool DataSemaphore::stop() {
    if ( this->pid_paused != 0 ) return false;
    this->sem.try_lock( this->pid );
    std::string aquiring_reason = std::string("WAIT ") + std::to_string(this->pid).c_str();
    if ( sem_trywait( this->sem.sem ) == 0 ) {
        if ( this->wait_lock.try_lock( this->pid_paused ) ) {
            this->pid_paused = this->pid;
            this->pid = 0;
        } else {
            callback_f cb = nullptr;
            callback_f cb_notify = nullptr;
            if ( this->current != nullptr ) {
                cb = this->current->callback;
                cb_notify = this->current->notifier;
            }
            process_identifier_t pident{ this->pid_paused, "PAUSING_RESUMING" };
            aquiring_lock_t al{ cb, this->pid_paused };
            time_aquiring_t ta{ DataSemaphore::make_timespec( time_t(0), -1L ), al };
            ProcessIdentifier* prident = new ProcessIdentifier( pident );
            prident->callback = cb;
            prident->notifier = cb_notify;

            prident->wait_time = systime_now();
            this->waitings.emplace( pident, prident );
        }
    } else return false;
    return this->sem.try_unlock( this->pid_paused );
}


bool DataSemaphore::goon() {
    if ( this->pid_paused == 0 ) return false;
    if ( this->sem.try_lock( this->pid_paused ) != 0 ) return false;
    this->wait_lock.try_unlock( this->pid_paused );
    if ( sem_post( this->sem.sem ) == 0 ) {
        this->pid = this->pid_paused;
        this->pid_paused = 0;
    } else return false;
    
    return this->sem.try_unlock( this->pid );
}

bool DataSemaphore::call( const time_aquiring_t ta ) {
    if ( ta.second.first == nullptr ) return false;

    // TODO: function name with <source_location>
    std::string aquiring_reason
        = std::string("CALL 'function'.");

    sem_t* sm = this->aquire_semaphore( ta.second.second, aquiring_reason );

    if ( ta.first != nullptr ) {
        if ( ta.first->tv_nsec > -1 || ta.first->tv_sec >= 0.0 ) {
            if ( sem_timedwait(sm, ta.first ) == 0
              && sem_timedwait( this->dsem.sem, ta.first ) == 0 )
                goto cb_wait_suceed;
            else return false;
        } else goto cb_wait_suceed;

    } else {
        if ( this->dlocked() && ta.first == nullptr ) {
            struct timespec* tspec = DataSemaphore::make_timespec( time_t(1), 0L );
            if ( sem_timedwait( this->dsem.sem, tspec ) == 0 )
                goto cb_wait_suceed;
            else return false;
        } else goto cb_wait_suceed;
    }

cb_wait_suceed:
    this->aquire_semaphore( ta.second.second, aquiring_reason );
    
    const bool resume_required = ! this->dsem.try_lock( ta.second.second );
    
    if ( resume_required ) {
        this->stop();
        goto cb_now;
    }

cb_now:
    ta.second.first( this, this->dsem.mtx, sm, this->dsem.handle );
    goto cb_finish;
    
cb_finish:
    if ( ! this->unaquire_semaphore( this->dsem.process_id(), aquiring_reason ) ) return false;
    else if( resume_required ) return this->goon();

    process_identifier_t pident{ ta.second.second, aquiring_reason };
    return this->notify( pident );
}

} // namespace psem