#include "translation_unit.h"

translation_unit_t *make_translation_unit(rdonlymsg_t * from,
                                          rw_msg_t * to,
                                          symbol_table_t * global_vars)
{
    translation_unit_t *ret = malloc(sizeof *ret);
    ret->from = from;
    ret->to = to;
    ret->global_vars = global_vars;
    ret->local_vars = make_symbol_table();
    return ret;
}

void free_translation_unit(translation_unit_t * unit)
{
    unit->local_vars->free(unit->local_vars);
    /* Do NOT free the messages, they are used elsewhere. */
    free(unit);
}
