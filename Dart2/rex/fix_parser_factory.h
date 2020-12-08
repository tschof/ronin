/*************************************************************************
 * Copyright (c) 2013 by DART executions, LLC
 *
 * All Rights Reserved
 * ***********************************************************************/

#ifndef FIX_PARSER_FACTORY_H__
#define FIX_PARSER_FACTORY_H__

#if defined(__cplusplus)
extern "C" {
#endif

#include "fix_parser.h"
#include "fix_trans.h"
#include "dart_order_obj.h"
#include "log_mnger.h"

fixp* create_fix_parse_block(void* pass_back_pointer,
                        void (*call_back)(void* pbp, dart_order_obj* doj),
                        otl* o,char* config_directory, int dir_len,
                        int num_dart_ord_objs);

#if defined(__cplusplus)
}
#endif
#endif
