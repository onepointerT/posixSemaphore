// Copyright (C) 2026 The OnePointer Authors.
//

#include "parampack.h"

#include <stdlib.h>
#include <string.h>


#define PARAM_PACK_MAX_SIZE 47


struct ParamPack* param_pack_new( const size_t psize ) {
    struct ParamPack* pp = (struct ParamPack*) malloc(sizeof(struct ParamPack));

    size_t size = psize;
    if ( psize > PARAM_PACK_MAX_SIZE ) {
        size = PARAM_PACK_MAX_SIZE;
    }

    pp->size = size;
    pp->arguments = (char**) malloc(sizeof(char*)*size);
    pp->parameters = (char**) malloc(sizeof(char*)*size);
    for ( unsigned int p = 0; p < pp->size; p++ ) {
        pp->arguments[p] = NULL;
        pp->parameters[p] = NULL;
    }

    return pp;
}

size_t param_pack_find( const struct ParamPack* pp, const char* param ) {
    for ( unsigned int p = 0; p < pp->size; p++ ) {
        if ( strcmp( pp->parameters[p], param) == 0 )
            return p;
    }
    return pp->size;
}


size_t param_pack_find_first_empty( const struct ParamPack* pp ) {
    for ( unsigned int p = 0; p < pp->size; p++ ) {
        if ( pp->parameters[p] == NULL ) return p;
    }
    return pp->size;
}


bool param_pack_set( struct ParamPack* pp, const char* param, const char* value ) {
    size_t pos = param_pack_find( pp, param );
    if ( pos == pp->size ) {
        size_t pos_first_empty = param_pack_find_first_empty( pp );
        if ( pos_first_empty == pp->size ) return false;
        pos = pos_first_empty;
    }    

    pp->parameters[pos] = (char*) malloc(sizeof(char)*strlen(param));
    strcpy( pp->parameters[pos], param );
    pp->arguments[pos] = (char*) malloc(sizeof(char)*strlen(value));
    strcpy( pp->arguments[pos], value );

    return true;
}

char* param_pack_get_param( const struct ParamPack* pp, const size_t pos ) {
    if ( pos >= pp->size ) return "";
    return pp->parameters[pos];
}

char* param_pack_get_value( const struct ParamPack* pp, const char* param ) {
    size_t pos = param_pack_find( pp, param );
    return param_pack_get_valuepos( pp, pos );
}

char* param_pack_get_valuepos( const struct ParamPack* pp, const size_t pos ) {
    if ( pos >= pp->size ) return "";
    return pp->arguments[pos];
}
