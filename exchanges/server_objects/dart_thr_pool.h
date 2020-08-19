/**************************************************************************
 * Copyright (c) 2008 by Ronin Capital, LLC
 *
 * All Rights Reserved
 *************************************************************************/

#ifndef _DART_THR_POOL_H__
#define _DART_THR_POOL_H__

#if defined(__cplusplus)
extern "C" {
#endif
#include <pthread.h>

    typedef struct thr_pool thr_pool_t;

    thr_pool_t *thr_pool_create(int min_threads, int max_threads,
                                int linger, int *cpus, int set_affinity);

    int thr_pool_queue(thr_pool_t * pool, void *(*func) (void *),
                       void *arg);

    void destroy_dart_thr_pool(thr_pool_t * pool);

#if defined(__cplusplus)
}
#endif
#endif                          //DART_THR_POOL_H__
