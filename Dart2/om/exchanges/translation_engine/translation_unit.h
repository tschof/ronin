#ifndef __TRANSLATION_UNIT_H__
#define __TRANSLATION_UNIT_H__

#include "rdonly_msg.h"
#include "symbol_table.h"
#include "rw_msg.h"

typedef struct translation_unit {
    symbol_table_t *global_vars, *local_vars;
    rdonlymsg_t *from;
    rw_msg_t *to;
} translation_unit_t;

translation_unit_t *make_translation_unit(rdonlymsg_t *, rw_msg_t *,
                                          symbol_table_t *);
void free_translation_unit(translation_unit_t *);

#endif
