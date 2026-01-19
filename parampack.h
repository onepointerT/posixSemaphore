// Copyright (C) The OnePointer Authors.
//

#ifndef _PARAMPACK_H_
#define _PARAMPACK_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>


typedef struct ParamPack {
    char** parameters;
    char** arguments;
    size_t size;
} param_pack_t;


struct ParamPack* param_pack_new( const size_t psize );
size_t param_pack_find( const struct ParamPack* pp, const char* param );
size_t param_pack_find_first_empty( const struct ParamPack* pp );
bool param_pack_set( struct ParamPack* pp, const char* param, const char* value );
char* param_pack_get_param( const struct ParamPack* pp, const size_t pos );
char* param_pack_get_value( const struct ParamPack* pp, const char* param );
char* param_pack_get_valuepos( const struct ParamPack* pp, const size_t pos );


#ifdef __cplusplus
}
#endif


#endif // _PARAMPACK_H_