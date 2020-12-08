/**************************************************************************
 * Copyright (c) 2008 by Ronin Capital, LLC
 *
 * All Rights Reserved
 *************************************************************************/
#ifndef _REX_THR_POOL_H__
#define _REX_THR_POOL_H__
#if defined(__cplusplus)
extern "C" {
#endif

#include "log_mnger.h"

typedef struct thr_pool thr_pool_t;

int create_thr(thr_pool_t* pool,  void (*func) (void *), void *arg);
thr_pool_t *thr_pool_create(otl* o, int min_threads, int max_threads,
                            int linger, int *cpus);
int thr_pool_queue(thr_pool_t * pool, void (*func) (void *), void *arg);
void one_stop_pop(thr_pool_t * pool);
void thr_pool_destroy(thr_pool_t * pool);
void thr_pool_wait_n_join(thr_pool_t* pool);

#if defined(__cplusplus)
}
#endif
#endif
