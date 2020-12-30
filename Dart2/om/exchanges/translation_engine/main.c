#include "writeable_fix_message.h"
#include "util.h"

#include <stdlib.h>
#include <sys/uio.h>
#include <string.h>
#include <stdint.h>
#include <assert.h>
#include <stdio.h>

struct readable_message {
    struct iovec (*get) (struct readable_message *, size_t);
};

struct scratchpad;

struct translation_op {
    int (*op) (struct translation_op *, struct readable_message *,
               struct writeable_message *, struct scratchpad *,
               struct iovec * output);
    void (*free) (void *);
};

typedef struct translation_op trans_op_t;

struct direct_translation {
    trans_op_t parent;
    size_t from;
};

int
direct_translation(trans_op_t * op, struct readable_message *in,
                   struct writeable_message *out,
                   struct scratchpad *scratch, struct iovec *output)
{
    struct direct_translation *dt = (struct direct_translation *) op;
    *output = in->get(in, dt->from);
    return 0;
}

trans_op_t *make_direct_translation(size_t from_field)
{
    struct direct_translation *ret = malloc(sizeof *ret);
    ret->parent.op = direct_translation;
    ret->parent.free = free;
    ret->from = from_field;
    return (trans_op_t *) ret;
}

int main(void)
{
    size_t layout[] = {
        1, 11, 60, 55, 65, 54, 114, 38, 111, 40, 44, 9622, 211, 110,
        47, 59, 126, 18, 7928, 9303, 9479, 439, 440, 9618, 9732
    };
    wrmsg_t *p = make_writeable_fix_message(layout,
                                            sizeof layout /
                                            sizeof layout[0]);
    assert(p != 0);
    assert(p->set != 0);
    if (p->set != 0) {
        assert(p->set(p, 11, make_iovec_fromstr("JK2DD4K", 7)) != -1);
        assert(p->set(p, 439, make_iovec_fromstr("0501", 4)) != -1);
    }
    p->dump(p, stdout);
    printf("all tests passed\n");
    return 0;
}
