// Copyright (C) 2026 The OnePointer Authors.
//

#ifndef _TRUTH_H_
#define _TRUTH_H_


#ifdef __cplusplus
extern "C"  {
#endif

#include <stdbool.h>
#include <stdint.h>


typedef enum {
    AND,
    OR,
    IMPLIES,
    EQUIV,
    UNEQ,
    NEGATION
} truth_operators_t;


typedef union TruthValue {
    char* str;
    void* ptr;
    bool* val;
    int* num;
    double* dbl;
} truth_value_t;

truth_value_t* truthvalue_new();
truth_value_t truthvalue_new_str( char* str );
truth_value_t truthvalue_new_ptr( void* dataptr, const char* type );
truth_value_t truthvalue_new_boolval( bool* boolptr );
truth_value_t truthvalue_new_num( int* intptr );
truth_value_t truthvalue_new_double( double* doubleptr );

bool trutheval_str( const char* lhs, const enum truth_operators_t op, const char* rhs );
bool trutheval_bool( const bool* lhs, const enum truth_operators_t op, const bool* rhs );
bool trutheval_num( const int* lhs, const enum truth_operators_t op, const int* rhs );
bool trutheval_double( const double* lhs, const enum truth_operators_t op, const double* rhs );
bool trutheval_ptr( const void* lhs, const enum truth_operators_t op, const void* rhs, const char* type );

bool trutheval( truth_value_t lhs, enum truth_operators_t op, truth_value_t rhs );

extern struct VoidEvaluatingTruth {
    const char* struct_type;

    bool (*trutheval_voidptr)(const void* lhs, const enum truth_operators_t op, const void* rhs);
};

struct VoidEvaluatingTruth* voidevauluatingtruth_new( const char* struct_type );
extern bool trutheval_voidptr( const void* lhs, const enum truth_operators_t op, const void* rhs );

typedef struct Truth {
    truth_value_t val1;
    enum truth_operators_t op;
    truth_value_t val2;
} truth_t;

struct Truth* truth_new( truth_value_t val1, enum truth_operators_t op, truth_value_t val2 );
bool trutheval_truth( const struct Truth* truth );

typedef struct TruthEquivation {
    struct Truth** equivs;
    enum truth_operators_t** ops;
    size_t size;
    size_t size_used;
} truth_equivation_t;

struct TruthEquivation* truth_equivation_new( const size_t arr_size );
bool truth_equivation_push( struct TruthEquivation* te, struct Truth* truth, enum truth_operators_t opright );

bool trutheval_equivation( const struct TruthEquivation* equiv );


#ifdef __cplusplus
}
#endif


#endif // _TRUTH_H_