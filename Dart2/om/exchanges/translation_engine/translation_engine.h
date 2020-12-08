#ifndef __TRANSLATION_ENGINE_H__
#define __TRANSLATION_ENGINE_H__

#include "translation_unit.h"
#include "inputstream.h"
#include "proto-rule-parser.h"
#include "message_layout.h"

struct translation_engine;
typedef struct translation_engine translation_engine_t;

struct translation_engine {
    int (*translate) (translation_engine_t *, translation_unit_t *);
    void (*free) (translation_engine_t *);

    symbol_table_t *global_vars;
    message_layout_t *output_layout;
    rule_t *rules;
};

translation_engine_t *make_translation_engine(istream_t *);

#endif
