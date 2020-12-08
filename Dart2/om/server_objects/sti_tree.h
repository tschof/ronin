/**************************************************************************
 * Copyright (c) 2008 by DART Executions, LLC
 *
 * All Rights Reserved
 *************************************************************************/
#ifndef _STI_TREE_H_
#define _STI_TREE_H_
#if defined(__cplusplus)
extern "C" {
#endif

typedef struct sti_node sti_node_t;
typedef struct sti_tree sti_tree_t;
/**
 * Static short tree, will carve out as many nodes
 * as it can from the memory provided. The 
 * sti_tree_t structure is 24 bytes and 
 * each node is 40 bytes. 2 nodes are burned
 * for the root and "nil" node so use that in
 * your calculation when deciding how much 
 * memory to use.
 */
sti_tree_t *create_sti_tree(void* mem, size_t len);
/**
 * Returns the node in which the value was inserted or
 * 0 if it has run out of free nodes.
 * Nothing is deleted
 */
sti_node_t *sti_tree_insert(sti_tree_t *, short key, short val);
void sti_tree_print(sti_tree_t *);
/**
 * Returns -1 on failure so this tree will not work for
 * storing negative values. 
 */
short sti_tree_get_val(sti_tree_t *, short key);
/**
 * If the key is found it is removed and the node
 * is put on the free list.
 */
void sti_tree_remove(sti_tree_t *, short key);

#if defined(__cplusplus)
}
#endif
#endif

