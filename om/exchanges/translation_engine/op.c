#include <stdint.h>
#include <string.h>
#include "op.h"
#include "opnode.h"
#include "rule.h"
#include "../../server_objects/hashmap.h"

struct direct_op {
    op_t parent;
    size_t field;
};

static int direct_op_translate(op_t * op, translation_unit_t * state,
                               scalar_t * result)
{
    struct direct_op *direct_op = (struct direct_op *) op;
    *result = state->from->get(state->from, direct_op->field);
    return 0;
}

op_t *make_direct_op(size_t field)
{
    struct direct_op *ret = malloc(sizeof *ret);
    if (ret) {
        ret->parent.translate = direct_op_translate;
        ret->parent.free = free;
        ret->field = field;
    }
    return (op_t *) ret;
}

struct string_constant_op {
    op_t parent;
    struct iovec constant;
};

static void string_constant_free(void *p)
{
    struct string_constant_op *op = p;
    if (op != 0) {
        free(op->constant.iov_base);
    }
    free(op);
}

static int string_constant_op_translate(op_t * op,
                                        translation_unit_t * state
                                        __attribute__ ((unused)),
                                        scalar_t * result)
{
    struct string_constant_op *const_op = (struct string_constant_op *) op;
    if (const_op != 0) {
        *result = const_op->constant;
    }
    return 0;
}

op_t *make_string_constant_op(const char *str)
{
    struct string_constant_op *ret = malloc(sizeof *ret);
    if (ret != 0) {
        ret->parent.translate = string_constant_op_translate;
        ret->parent.free = string_constant_free;
        ret->constant.iov_base = strdup(str);
        ret->constant.iov_len = strlen(ret->constant.iov_base);
    }
    return (op_t *) ret;
}

/* The lookupmap function does just that, takes the input string and
 * looks it up into the global vars map and returns the output. */
struct lookupmap_op {
    op_t parent;
    char *mapname;
};

static int lookupmap_translate(op_t * op, translation_unit_t * unit,
                               scalar_t * result)
{
    if (op != 0) {
        struct lookupmap_op *lookupmap = (struct lookupmap_op *) op;
        if (result->iov_len > 0 && result->iov_base != 0) {
            void *data;
            if (get_map_entry
                (unit->global_vars->maps, lookupmap->mapname,
                 strlen(lookupmap->mapname), &data)) {
                hashmap_t map = (hashmap_t) data;
                if (get_map_entry
                    (map, result->iov_base, result->iov_len, &data)) {
                    char *p = data;
                    result->iov_base = p;
                    result->iov_len = strlen(p);
                }
            }
        }
    }
    return 0;
}

op_t *make_lookupmap_op(char *varname)
{
    struct lookupmap_op *ret = malloc(sizeof *ret);
    ret->parent.translate = lookupmap_translate;
    ret->parent.free = free;
    ret->mapname = varname;
    return (op_t *) ret;
}

struct split_op {
    op_t parent;
    uint16_t which_field;
    char sep;
};

static int split_op_translate(op_t * op, translation_unit_t * state
                              __attribute__ ((unused)), scalar_t * val)
{
    int ret = -1;
    if (val->iov_len == 0) {
        return 0;
    }
    struct split_op *split = (struct split_op *) op;
    uint16_t i;
    char *ptr = val->iov_base;
    size_t len = val->iov_len;
    for (i = 0; i <= split->which_field; ++i) {
        char *p = memchr(ptr, split->sep, len);
        if (i == split->which_field) {
            if (p == 0) {
                p = ptr + len;
            }
            val->iov_len = p - ptr;
            val->iov_base = ptr;
            ret = 0;
            break;
        } else if (p == 0) {
            val->iov_len = 0;
            break;
        } else {
            len = len - (p - ptr) - 1;
            ptr = p + 1;
        }
    }
    return ret;
}

op_t *make_split_op(uint16_t which_field, char sep)
{
    struct split_op *ret = malloc(sizeof *ret);
    if (ret != 0) {
        ret->parent.translate = split_op_translate;
        ret->parent.free = free;
        ret->which_field = which_field;
        ret->sep = sep;
    }
    return (op_t *) ret;
}

struct map_op {
    op_t parent;
    hashmap_t map;
    opnode_t *key_not_found_op;
};

static int map_op_translate(op_t * op, translation_unit_t * state
                            __attribute__ ((unused)), scalar_t * in_out)
{
    int ret = -1;
    struct map_op *map_op = (struct map_op *) op;
    if (map_op != 0) {
        void *result;
        ret =
            get_map_entry(map_op->map, in_out->iov_base,
                          (int) in_out->iov_len, &result);
        if (ret > 0) {
            in_out->iov_base = result;
            in_out->iov_len = strlen(in_out->iov_base);
            ret = 0;
        } else if (map_op->key_not_found_op) {
            ret = run_operations(map_op->key_not_found_op, state, in_out);
        }
    }
    return ret;
}

static void map_op_free(void *p)
{
    struct map_op *op = p;
    delete_map(op->map);
    free(op);
}

op_t *make_map_op(hashmap_t hashmap, opnode_t * key_not_found_op)
{
    struct map_op *op = malloc(sizeof *op);
    op->parent.translate = map_op_translate;
    op->parent.free = map_op_free;
    op->map = hashmap;
    op->key_not_found_op = key_not_found_op;
    return (op_t *) op;
}

#ifdef COMPILE_UNIT_TESTS
#include "../../unit-tester/unit-tester.h"
#include "rdonly_csv_msg.h"
#include "rw_fix_msg.h"
#include <string.h>

#define narray(array) (sizeof (array) / sizeof (array)[0])

declare_unit_test(basic_op)
{
    char *raw_csv_message =
        strdup
        ("E,,FIXH3TRA,11856000711,1,BRK/B,953,71.2,2,5,A,501,AFH21209,145,,RND,,FIXH3TRA-90130503-275,,,,,H3TRA,501,H3TRA,,,FIXH3TRA,,,,,,,,E,,,,,,145,,,,,,,,,20130503-12:03:30,,,,,,USD,,,,,,,,,,,,,,,,,,,,,,,foo\n");
    rdonlymsg_t *csv = make_dartcsv_rdonly_msg(raw_csv_message);
    check(csv != 0);
    size_t bats_fix_layout[] = {
        1, 11, 60, 55, 65, 54, 114, 38, 111, 40, 44, 9622, 211, 110,
        47, 59, 126, 18, 7928, 9303, 9479, 439, 440, 9618, 9732
    };
    rw_msg_t *fix =
        make_rw_fix_msg(make_message_layout((size_t *) & bats_fix_layout,
                                            narray(bats_fix_layout)));
    check(fix != 0);
    if (csv != 0 && fix != 0) {
        /* Let's make a few operation chains */
        opnode_t *node = 0;
        push_opnode(make_direct_op(12), &node);
        rule_t *rules = 0;
        push_rule(node, 1, &rules);
        node = 0;
        push_opnode(make_direct_op(5), &node);
        push_opnode(make_split_op(0, '/'), &node);
        push_rule(node, 55, &rules);
        node = 0;
        push_opnode(make_direct_op(5), &node);
        push_opnode(make_split_op(1, '/'), &node);
        push_rule(node, 65, &rules);
        fix->dump(fix, stdout);
        puts("\n");
        free_rules(rules);
        csv->free(csv);
        fix->free(fix);
    }
    free(raw_csv_message);
}

#endif
