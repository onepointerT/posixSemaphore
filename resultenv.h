// Copyright (C) 2026 The OnePointer Authors.
//

#ifndef _RESULTENV_H_
#define _RESULTENV_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>


#define ENVIRONMENT_MIN_SIZE 47
#define ENVIRONMENT_MEDIAN_SIZE 102
#define ENVIRONMENT_MAX_SIZE 487


typedef struct OwnerListPID {
    unsigned short** process_ids; // The processes using this,
                                // if an index is not given, the mutex is unlocked
    size_t** size_process_ids; // The size of the `process_id` arrays
} owner_list_pid_t;


struct OwnerListPID* ownerlistpid_new( const size_t length_arr_list );
bool ownerlistpid_mutex_unlocked( const size_t var_index );
bool ownerlistpid_add_processid( const size_t var_index );
bool ownerlistpid_is_reserved_for( const size_t var_index, const unsigned short process_id );
bool ownerlistpid_remove_processid( const size_t var_index, const unsigned short process_id );


typedef struct Environment {
    char** varnames; // The names 
    char** values;
    size_t size;

    struct OwnerListPID* pid_owners;
} environtment_t;

struct Environment* environment_new( const size_t size_var_arr );
bool environment_add_variable( const char* varname, const char* value, bool overwrite );
char* environment_get_variable( const char* varname );
unsigned short* environment_get_usersof( const char* varname );


typedef struct ResultEnvironment {
    struct Environment* env; // The results
    unsigned short process_id_originator;
    unsigned short process_id_parent;
} result_environment_t;

struct ResultEnvironment* resultenv_new( const size_t size_var_arr
                                , const unsigned short process_id_originator );


#ifdef __cplusplus
}
#endif

#endif // _RESULTENV_H_