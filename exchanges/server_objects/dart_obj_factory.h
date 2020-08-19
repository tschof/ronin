/**************************************************************************
 * Copyright (c) 2008 by Ronin Capital, LLC
 *
 * All Rights Reserved
 *************************************************************************/

#ifndef _DART_OBJ_FACTORY_H__
#define _DART_OBJ_FACTORY_H__

#include "message_queue.h"
#if defined(__cplusplus)
extern "C" {
#endif


    typedef struct dart_obj_fact dof_t;

    dof_t *create_dart_obj_fact(int initial_amt, void destroy_func(void *),
                                void create_func(queue_t glob, queue_t q,
                                                 int num),
                                void clear_obj(void *obj));

    void add_to_free_list(dof_t * d, int num);

    void *get_obj(dof_t * d);

    void destroy_dart_obj_fact(dof_t * d);

    void destroy_elem(dof_t * d, void *e);

#if defined(__cplusplus)
}
#endif
#endif                          //_MAILER_H__
