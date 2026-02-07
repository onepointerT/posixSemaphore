// Copyright (C) 2026 The OnePointer Authors.
//

extern "C" {
#include "mutex.h"
#include "ptime.h"
#include "semaphore.h"
}

#define BS_THREAD_POOL_NATIVE_EXTENSIONS
#include "BS_thread_pool.hpp"

#include <future>
#include <mutex>
#include <queue>
#include <string>
#include <typeindex>
#include <typeinfo>
#include <type_traits>
#include <unordered_map>
#include <utility>


namespace psem {


typedef unsigned short PID;
typedef HANDLE DHANDLE;


class semaphore_t final
    :   public arch_sem_t
{
    static unsigned int sid;

    std::string sname;
public:
    mutable sem_t* sem;
    mutable mutex_t* mtx;

    semaphore_t( void* data = NULL, const char* name = "" );

    bool try_lock( const PID process_id );
    bool try_unlock( const PID process_id );

    DHANDLE data() const;

    const PID process_id() const;
    const bool locked() const;

    const std::string name() const;
};


class DataSemaphore final
    :   public BS::binary_semaphore
{
protected:
    mutable semaphore_t dsem;

    const bool dlocked() const;
    
    mutable PID pid_paused;
    mutable std::string aquiring_reason_paused;

    
    static struct timespec* make_timespec( const time_t seconds, const long nanoseconds );

public:
    typedef typename PID (*callback_f)(BS::binary_semaphore*, mutex_t*, sem_t*, void*); /* Callback function for aquiring of locks */
    typedef typename std::pair< callback_f, PID > aquiring_lock_t;
    typedef typename std::pair< struct timespec*, aquiring_lock_t > time_aquiring_t;
    
    enum ProcessAbility { PROCESS_SHARED, PROCESS_PRIVATE };

    mutable semaphore_t sem;
    PID pid;
    std::string aquiring_reason;
    enum ProcessAbility dshared;
    std::type_info* dtype;

    bool is_free() const;
    bool is_lockable() const;
    bool is_available() const;

    template< typename D >
    D* getData() {
        if ( this->dtype == nullptr ) return this->sem.handle;
        return static_cast<decltype(std::declval(std::type_index(*this->dtype).name()))*>( this->dsem.handle );
    }

    template< typename D >
    void setData( D* data ) {
        if ( this->dlocked() ) return;
        this->dtype = new std::type_info( typeid(*data) );
        this->dsem.handle = data;
    }

    template<>
    void setData( void* data ) {
        if ( this->dlocked() ) return;
        this->dtype = nullptr;
        this->dsem.handle = data;
    }

    typedef typename std::pair< PID, std::string > process_identifier_t;

    class ProcessIdentifier
        :   public process_identifier_t
    {
    public:
        DataSemaphore::callback_f callback;
        DataSemaphore::callback_f notifier;
        
        bool paused;
        struct ProcessTime* wait_time;

        ProcessIdentifier( const PID process_id, const std::string aquiring_reason );
        ProcessIdentifier( const process_identifier_t pident );

        bool empty() const;

        operator PID() const;
        operator std::string() const;

        PID call( BS::binary_semaphore* lock, mutex_t* mt, sem_t* sema, void* data ) const;
        PID notify( BS::binary_semaphore* lock, mutex_t* mt, sem_t* sema, void* data ) const;
    };

    ProcessIdentifier* current;

    typedef typename std::unordered_multimap< process_identifier_t, time_aquiring_t > process_map_t;
    typedef typename std::unordered_multimap< PID, ProcessIdentifier* > fulfillment_map_t;
    typedef typename std::queue< std::pair< process_identifier_t, time_aquiring_t > > process_queue_t;

    semaphore_t wait_lock;
    process_map_t waitings;
    fulfillment_map_t fulfillment_callbacks;

    process_identifier_t find_waiter( const PID process_id ) const;
    time_aquiring_t find_waiting_aquirement( const process_identifier_t pident );
    process_queue_t find_waiter_by_aquirementReason( const std::string aquirement_reason ) const;
    callback_f find_fulfillment( const PID process_id ) const;

    DataSemaphore( void* data = NULL, const char* name = "" );

    bool lock( const PID process_id, const enum ProcessAbility pshared );
    bool unlock( const PID process_id );

    sem_t* aquire_semaphore( const PID process_id, const std::string aquiring_reason
            , callback_f callback = NULL, struct timespec* waiting_time = NULL );
    bool unaquire_semaphore( const PID process_id, const std::string aquiring_reason );

    typedef typename std::pair< PID, void* > result_t;

    result_t wait_for( const time_t seconds, const long nanoseconds, const PID process_id
                , callback_f callback, callback_f fulfillment_callback = NULL, bool callit = true );

    bool stop();
    bool goon();

    bool notify( const process_identifier_t pident );
    bool call( const time_aquiring_t ta );
};


} // namespace psem