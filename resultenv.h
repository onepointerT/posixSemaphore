// Copyright (C) 2026 The OnePointer Authors.
//

#ifndef _RESULTENV_H_
#define _RESULTENV_H_

#ifdef __cplusplus
extern "C" {
#endif


typedef struct Environment {
    char** varnames;
    char** values;
    size_t size;
} environtment_t;


typedef struct ResultEnvironment {
    struct Environment* env;
    unsigned long*** process_ids;
    size_t** size_process_ids;
    unsigned long process_id_originator;
} result_environment_t;

#ifdef __cplusplus
}
#endif

#endif // _RESULTENV_H_