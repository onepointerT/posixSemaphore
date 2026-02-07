// Copyright (C) 2026 The OnePointer Authors.
//

#include "truth.h"

#include <stdlib.h>
#include <string.h>


truth_value_t* truthvalue_new() {
    truth_value_t* tvt = (union TruthValue*) malloc(sizeof(union TruthValue));

    tvt->dbl = NULL;
    tvt->num = NULL;
    tvt->ptr = NULL;
    tvt->str = NULL;
    tvt->val = NULL;

    return tvt;
}

truth_value_t truthvalue_new_str( char* str ) {
    truth_value_t* tvt = truthvalue_new();

    tvt->str = str;

    return *tvt;
}


truth_value_t truthvalue_new_ptr( void* dataptr, const char* type ) {
    truth_value_t* tvt = truthvalue_new();

    tvt->ptr = dataptr;
    tvt->str = type;

    return *tvt;
}


truth_value_t truthvalue_new_boolval( bool* boolptr ) {
    truth_value_t* tvt = truthvalue_new();

    tvt->val = boolptr;

    return *tvt;
}


truth_value_t truthvalue_new_num( int* intptr ) {
    truth_value_t* tvt = truthvalue_new();

    tvt->num = intptr;

    return *tvt;
}


truth_value_t truthvalue_new_double( double* doubleptr ) {
    truth_value_t* tvt = truthvalue_new();

    tvt->dbl = doubleptr;

    return *tvt;
}



bool trutheval_str( const char* lhs, const enum truth_operators_t op, const char* rhs ) {
    if ( ( lhs == NULL || rhs == NULL ) && op != NEGATION ) return false;

    if ( op == AND ) { // Bitwise AND
        return *lhs && *rhs;
    } else if ( op == OR ) { // Bitwise OR
        return *lhs || *rhs;
    } else if ( op == IMPLIES ) {
        return ( *lhs && *rhs ) || *lhs;
    } else if ( op == EQUIV ) {
        return strcmp( lhs, rhs ) == 0;
    } else if ( op == UNEQ ) {
        return strcmp( lhs, rhs ) != 0;
    } else if ( op == NEGATION ) {
        if ( lhs != NULL && rhs == NULL ) return ! *lhs;
        else if ( rhs != NULL && lhs != NULL ) return ! *rhs;
        return false;
    } else return false;
}


bool trutheval_bool( const bool* lhs, const enum truth_operators_t op, const bool* rhs ) {
    if ( ( lhs == NULL || rhs == NULL ) && op != NEGATION ) return false;

    if ( op == AND ) { // Bitwise AND
        return *lhs && *rhs;
    } else if ( op == OR ) { // Bitwise OR
        return *lhs || *rhs;
    } else if ( op == IMPLIES ) {
        return (! *lhs) || rhs;
    } else if ( op == EQUIV ) {
        return *lhs == *rhs;
    } else if ( op == UNEQ ) {
        return *lhs != *rhs;
    } else if ( op == NEGATION ) {
        if ( lhs == NULL && rhs != NULL ) return ! *rhs;
        else if ( rhs == NULL && lhs != NULL ) return ! *lhs;
        return false;
    } else return false;
}


bool trutheval_num( const int* lhs, const enum truth_operators_t op, const int* rhs ) {
    if ( ( lhs == NULL || rhs == NULL ) && op != NEGATION ) return false;

    if ( op == AND ) { // Bitwise AND
        return *lhs && *rhs;
    } else if ( op == OR ) { // Bitwise OR
        return *lhs || *rhs;
    } else if ( op == IMPLIES ) {
        return trutheval_num( *lhs && *rhs, NEGATION, NULL ) || *rhs;
    } else if ( op == EQUIV ) {
        return *lhs == *rhs;
    } else if ( op == UNEQ ) {
        return *lhs != *rhs;
    } else if ( op == NEGATION ) {
        if ( lhs != NULL && rhs == NULL ) return ! *lhs;
        else if ( rhs != NULL && lhs == NULL ) return ! *rhs;
        else return false;
    } else return false;
}


bool trutheval_double( const double* lhs, const enum truth_operators_t op, const double* rhs ) {
    if ( ( lhs == NULL || rhs == NULL ) && op != NEGATION ) return false;

    if ( op == AND ) { // Bitwise AND
        return *lhs && *rhs;
    } else if ( op == OR ) { // Bitwise OR
        return *lhs || *rhs;
    } else if ( op == IMPLIES ) {
        return trutheval_double( *lhs || *rhs, NEGATION, NULL ) && *lhs;
    } else if ( op == EQUIV ) {
        return *lhs == *rhs;
    } else if ( op == UNEQ ) {
        return *lhs != *rhs;
    } else if ( op == NEGATION ) {
        if ( rhs == NULL && lhs != NULL ) return ! *lhs;
        else if ( lhs == NULL && rhs != NULL ) return ! *rhs;
        else return false;
    } else return false;
}


bool trutheval_ptr( const void* lhs, const enum truth_operators_t op, const void* rhs, const char* type ) {
    if ( ( lhs == NULL || rhs == NULL ) && op != NEGATION ) return false;
    else if ( strcmp(type, ((struct VoidEvaluatingTruth*) lhs)->struct_type) != 0
        || strcmp(type, ((struct VoidEvaluatingTruth*) rhs)->struct_type) != 0 ) return false;
    else return ((struct VoidEvaluatingTruth*) lhs)->trutheval_voidptr(lhs, AND, rhs);
}




bool trutheval( truth_value_t lhs, enum truth_operators_t op, truth_value_t rhs ) {
    if ( lhs.dbl != NULL ) return trutheval_double(lhs.dbl, op, rhs.dbl);
    else if ( lhs.num != NULL ) return trutheval_num(lhs.num, op, rhs.num);
    else if ( lhs.ptr != NULL ) return trutheval_ptr(lhs.ptr, op, rhs.ptr, lhs.str);
    else if ( lhs.str != NULL ) return trutheval_str(lhs.str, op, rhs.str);
    else if ( lhs.val != NULL ) return trutheval_bool(lhs.val, op, rhs.val);
    else return false;
}


struct VoidEvaluatingTruth* voidevauluatingtruth_new( const char* struct_type ) {
    struct VoidEvaluatingTruth* vet = (struct VoidEvaluatingTruth*) malloc(sizeof(struct VoidEvaluatingTruth));

    vet->struct_type = struct_type;
    vet->trutheval_voidptr = &trutheval_voidptr;

    return vet;
}


struct Truth* truth_new( truth_value_t val1, enum truth_operators_t op, truth_value_t val2 ) {
    struct Truth* truth = (struct Truth*) malloc(sizeof(struct Truth));

    truth->val1 = val1;
    truth->op = op;
    truth->val2 = val2;

    return truth;
}


bool trutheval_truth( const struct Truth* truth ) {
    return trutheval( truth->val1, truth->op, truth->val2 );
}




struct TruthEquivation* truth_equivation_new( const size_t arr_size ) {
    struct TruthEquivation* te = (struct TruthEquivation*) malloc(sizeof(struct TruthEquivation));

    te->equivs = (struct Truth**) malloc(sizeof(struct Truth*)*arr_size);
    te->ops = (enum truth_operators_t**) malloc(sizeof(truth_operators_t*)*arr_size);
    te->size = arr_size;
    te->size_used = 0;

    for ( unsigned int fi = 0; fi < te->size; fi++ ) {
        te->equivs[fi] = NULL;
        te->ops[fi] = NULL;
    }

    return te;
}


bool truth_equivation_push( struct TruthEquivation* te, struct Truth* truth, enum truth_operators_t opright ) {
    if ( te->size_used >= te->size ) return false;

    for ( unsigned int fi = 0; fi < te->size; fi++ ) {
        if ( te->equivs[fi] == NULL ) {
            te->equivs[fi] = truth;
            te->ops[fi] = &opright;
            ++te->size_used;
            return true;
        }
    }

    return false;
}



bool trutheval_equivation( const struct TruthEquivation* equiv ) {
    if (equiv->equivs[0] == NULL) return false;

    bool current_result = trutheval_truth(equiv->equivs[0]);
    for ( unsigned int ti = 1; ti < equiv->size; ti++ ) {
        bool next_result = trutheval_truth(equiv->equivs[ti]);
        current_result = trutheval_bool( &current_result, *(equiv->ops[ti-1]), &next_result );
        if ( equiv->ops[ti] == NULL ) break;
    }

    return current_result;
}


