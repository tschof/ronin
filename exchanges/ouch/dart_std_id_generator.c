#include "dart_std_id_generator.h"
#include "ex_common/order_token_creator.h"

#include "dstring.h"

#include <stdlib.h>

int dart_standard_id_generator__new_id(struct id_generator *generator,
                                       struct dstring *id)
{
    struct dart_std_id_generator *std_generator =
        (struct dart_std_id_generator *) generator;
    int retval =
        create_order_token(std_generator->state, id->p, id->nalloc);
    id->nlen = retval;
    return retval;
}

struct id_generator *make_dart_standard_id_generator(void)
{
    struct dart_std_id_generator *retval = malloc(sizeof *retval);
    retval->super.generate_id = dart_standard_id_generator__new_id;
    retval->state = init_order_token_creator();
    return (struct id_generator *) retval;
}
