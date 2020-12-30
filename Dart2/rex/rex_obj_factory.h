/**************************************************************************
 * Copyright (c) 2008 by Ronin Capital, LLC
 *
 * All Rights Reserved
 *************************************************************************/
#ifndef _REX_OBJ_FACTORY_H__
#define _REX_OBJ_FACTORY_H__
#if defined(__cplusplus)
extern "C" {
#endif

#include "message_queue.h"

typedef struct rex_obj_fact dof_t;
dof_t *create_rex_obj_fact(int initial_amt, void destroy_func(void *),
                            void create_func(queue_t q,
                                             int num),
                            void clear_obj(void *obj));
void add_to_free_list(dof_t * d, int num);
void *get_obj(dof_t * d);
void destroy_rex_obj_fact(dof_t * d);
void destroy_elem(dof_t * d, void *e);

#if defined(__cplusplus)
}
#endif
#endif
