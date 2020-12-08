/**************************************************************************
 * Copyright (c) 2014 by DART Executions, LLC
 *
 * All Rights Reserved
 *************************************************************************/
#ifndef DART_ORDER_FACTORY_H__
#define DART_ORDER_FACTORY_H__
#if defined(__cplusplus)
extern "C" {
#endif

#include "databuf.h"
#include "dart_order_obj.h"

typedef struct dart_order_factory dartfact;

dartfact* create_dart_order_factory(unsigned int num_obj);

void clean_fact(dartfact*);

dart_order_obj* get_doj(dartfact*);

void reset_doj(dartfact*, dart_order_obj*);

struct db_node* get_new_buffer(dartfact*);

void dart_init_def_pos();
void dart_style_reset(dart_order_obj* doj);

#if defined(__cplusplus)
}
#endif
#endif //_ORDER_OBJ_FACTORY_H__
