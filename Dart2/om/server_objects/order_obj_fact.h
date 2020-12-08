/**************************************************************************
 * Copyright (c) 2014 by DART Executions, LLC
 *
 * All Rights Reserved
 *************************************************************************/
#ifndef _ORDER_OBJ_FACTORY_H__
#define _ORDER_OBJ_FACTORY_H__
#if defined(__cplusplus)
extern "C" {
#endif

#include "databuf.h"
#include "dart_order_obj.h"

typedef struct order_obj_factory oofact;

oofact* create_order_obj_factory(unsigned int num_obj);

void clean_fact(oofact*);

dart_order_obj* get_doj(oofact*);

void reset_doj(oofact*, dart_order_obj*);

struct db_node* get_new_buffer(oofact*);


#if defined(__cplusplus)
}
#endif
#endif //_ORDER_OBJ_FACTORY_H__
