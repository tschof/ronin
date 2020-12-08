/**************************************************************************
 * Copyright (c) 2008 by Ronin Capital, LLC
 *
 * All Rights Reserved
 *************************************************************************/
#ifndef _RB_TREE_H_
#define _RB_TREE_H_
#if defined(__cplusplus)
extern "C" {
#endif
#include <stdint.h>
typedef struct rb_node rb_node_t;
typedef struct rb_tree rb_tree_t;

rb_tree_t *create_rb_tree(int (*compare_func) (const void *, const void *),
                          void destroy_func(void *),
                          void destroy_info_func(void *),
                          void print_func(const void *),
                          void print_func_info(const void *));
rb_node_t *rb_tree_insert(rb_tree_t *, void *key, void *val);
rb_node_t *rb_tree_no_delete_insert(rb_tree_t * tree, void *key,
                                    void *val);
void rb_tree_nd64_insert(rb_tree_t* tree, uint64_t* key, void* val);
void rb_tree_print(rb_tree_t *);
void rb_tree_delete(rb_tree_t *, void *key);
void rb_tree_destroy(rb_tree_t *);
void *rb_tree_get_val(rb_tree_t *, void *key);
void rb_tree_remove(rb_tree_t *, void *key);

#if defined(__cplusplus)
}
#endif
#endif
