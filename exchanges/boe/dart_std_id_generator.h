#ifndef __DART_STD_ID_GENERATOR_H__
#define __DART_STD_ID_GENERATOR_H__

#include "id_generator.h"

struct dart_std_id_generator {
    struct id_generator super;
    struct token_struct *state;
};

/* This does not reallocate if necessary, and this should be okay for
 * now. In the future we can have it check the flags and reallocate if
 * necessary. That way it would be a generic solution. */
int dart_standard_id_generator__new_id(struct id_generator *generator,
                                       struct dstring *id);

struct id_generator *make_dart_standard_id_generator(void);

#endif
