/**************************************************************************
 * Copyright (c) 2008 by Ronin Capital, LLC
 *
 * All Rights Reserved
 *************************************************************************/
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "sti_tree.h"

struct sti_node {
    struct sti_node *left;
    struct sti_node *right;
    struct sti_node *parent;
    struct sti_node *free_next;
    short key;
    short info;
    int red;
};

struct sti_tree {
    sti_node_t *root;
    sti_node_t *nil;
    sti_node_t* free_nodes;
};

sti_tree_t *create_sti_tree(void* mem, size_t len)
{
    memset(mem, '\0', len);
    sti_tree_t *ret_tree = mem;
    ret_tree->nil = mem + sizeof(struct sti_tree);
    ret_tree->nil->parent = ret_tree->nil->left =
        ret_tree->nil->right = ret_tree->nil;
    ret_tree->nil->red = 0;
    ret_tree->nil->key = 0;
    ret_tree->root =mem + sizeof(struct sti_tree) + (sizeof(struct sti_node));
    ret_tree->root->parent = ret_tree->root->left = ret_tree->root->right = ret_tree->nil;
    ret_tree->root->key = 0;
    ret_tree->root->red = 0;
    long cur_off = (sizeof(struct sti_tree) + (2* sizeof(struct sti_node)));
    long len_left = len - cur_off;
    if(len_left > sizeof(struct sti_node)) {
        do {
            sti_node_t* x = mem + cur_off;
            if(ret_tree->free_nodes != 0) {
                sti_node_t* n = ret_tree->free_nodes;
                x->free_next = n;
            }
            ret_tree->free_nodes = x;
            cur_off += sizeof(struct sti_node);
            len_left -= sizeof(struct sti_node);
        } while(len_left >= sizeof(struct sti_node));
    }
    return ret_tree;
}

static void left_rotate(sti_tree_t * tree, sti_node_t * x)
{
    sti_node_t *y;
    sti_node_t *nil = tree->nil;

    y = x->right;
    x->right = y->left;
    if (y->left != nil) {
        y->left->parent = x;
    }
    y->parent = x->parent;

    if (x == x->parent->left) {
        x->parent->left = y;
    } else {
        x->parent->right = y;
    }
    y->left = x;
    x->parent = y;
}

static void right_rotate(sti_tree_t * tree, sti_node_t * y)
{
    sti_node_t *x;
    sti_node_t *nil = tree->nil;

    x = y->left;
    y->left = x->right;

    if (nil != x->right) {
        x->right->parent = y;
    }

    x->parent = y->parent;
    if (y == y->parent->left) {
        y->parent->left = x;
    } else {
        y->parent->right = x;
    }
    x->right = y;
    y->parent = x;
}

static void tree_insert_help(sti_tree_t * tree, sti_node_t * z)
{
    sti_node_t *x;
    sti_node_t *y;
    sti_node_t *nil = tree->nil;
    z->left = z->right = nil;
    y = tree->root;
    x = tree->root->left;
    while (x != nil) {
        y = x;
        if (x->key > z->key) { 
            x = x->left;
        } else {
            x = x->right;
        }
    }
    z->parent = y;
    if ((y == tree->root) || (y->key > z->key)) { 
        y->left = z;
    } else {
        y->right = z;
    }
}

static sti_node_t* get_next_free(sti_tree_t* tree)
{
    if(tree->free_nodes != 0) {
        sti_node_t* temp = tree->free_nodes;
        tree->free_nodes = temp->free_next;
        temp->free_next = 0;
        return temp;
    } else {
        return 0;
    }
}

sti_node_t *sti_tree_insert(sti_tree_t * tree, short key, short val)
{
    sti_node_t *y;
    sti_node_t *x;
    sti_node_t *new_node;

    x =  get_next_free(tree);
    if(x == 0) {
        return 0;
    }
    x->key = key;
    x->info = val;
    tree_insert_help(tree, x);
    new_node = x;
    x->red = 1;
    while (x->parent->red) {
        if (x->parent == x->parent->parent->left) {
            y = x->parent->parent->right;
            if (y->red) {
                x->parent->red = 0;
                y->red = 0;
                x->parent->parent->red = 1;
                x = x->parent->parent;
            } else {
                if (x == x->parent->right) {
                    x = x->parent;
                    left_rotate(tree, x);
                }
                x->parent->red = 0;
                x->parent->parent->red = 1;
                right_rotate(tree, x->parent->parent);
            }
        } else {
            y = x->parent->parent->left;
            if (y->red) {
                x->parent->red = 0;
                y->red = 0;
                x->parent->parent->red = 1;
                x = x->parent->parent;
            } else {
                if (x == x->parent->left) {
                    x = x->parent;
                    right_rotate(tree, x);
                }
                x->parent->red = 0;
                x->parent->parent->red = 1;
                left_rotate(tree, x->parent->parent);
            }
        }
    }
    tree->root->left->red = 0;
    return new_node;
}

static sti_node_t *sti_next_node(sti_tree_t * tree, sti_node_t * x)
{
    sti_node_t *y;
    sti_node_t *nil = tree->nil;
    sti_node_t *root = tree->root;

    if (nil != (y = x->right)) {
        while (y->left != nil) {
            y = y->left;
        }
        return y;
    } else {
        y = x->parent;
        while (x == y->right) {
            x = y;
            y = y->parent;
        }
        if (y == root) {
            return nil;
        }
        return y;
    }
}

static void sti_print_node(sti_tree_t * tree, sti_node_t * x)
{
    sti_node_t *nil = tree->nil;
    sti_node_t *root = tree->root;
    if (x != tree->nil) {
        sti_print_node(tree, x->left);
        printf("\nInfo=%d ", x->info);
        printf("  Key= %d", x->key);
        printf("  l->key = ");
        if (x->left == nil) {
            printf("NULL");
        } else {
            printf("%d", x->left->key);
        }
        printf(" r->key= ");
        if (x->right == nil) {
            printf("NULL");
        } else {
            printf("%d", x->right->key);
        }
        printf(" p->key= ");
        if (x->parent == root) {
            printf("ROOT_NULL");
        } else {
            printf("%d", x->parent->key);
        }
        printf(" red=%i\n", x->red);
        sti_print_node(tree, x->right);
    }
}

static int short_compare(short x, short y)
{
    if(x > y) {
        return 1;
    } else if(x == y) {
        return 0;
    } else {
        return -1;
    }
}

sti_node_t *sti_find_key(sti_tree_t * tree, short q)
{
    sti_node_t *x = tree->root->left;
    sti_node_t *nil = tree->nil;
    int comp_val;
    if (x == nil) {
        return 0;
    }
    comp_val = short_compare(x->key, q);
    while (0 != comp_val) {
        if (1 == comp_val) {
            x = x->left;
        } else {
            x = x->right;
        }
        if (x == nil) {
            return 0;
        }
        comp_val = short_compare(x->key, q);
    }
    return x;
}

static void repair_delete(sti_tree_t * tree, sti_node_t * x)
{
    sti_node_t *root = tree->root->left;
    sti_node_t *w;
    while ((!x->red) && (root != x)) {
        if (x == x->parent->left) {
            w = x->parent->right;
            if (w->red) {
                w->red = 0;
                x->parent->red = 1;
                left_rotate(tree, x->parent);
                w = x->parent->right;
            }
            if ((!w->right->red) && (!w->left->red)) {
                w->red = 1;
                x = x->parent;
            } else {
                if (!w->right->red) {
                    w->left->red = 0;
                    w->red = 1;
                    right_rotate(tree, w);
                    w = x->parent->right;
                }
                w->red = x->parent->red;
                x->parent->red = 0;
                w->right->red = 0;
                left_rotate(tree, x->parent);
                x = root;
            }
        } else {
            w = x->parent->left;
            if (w->red) {
                w->red = 0;
                x->parent->red = 1;
                right_rotate(tree, x->parent);
                w = x->parent->left;
            }
            if ((!w->right->red) && (!w->left->red)) {
                w->red = 1;
                x = x->parent;
            } else {
                if (!w->left->red) {
                    w->right->red = 0;
                    w->red = 1;
                    left_rotate(tree, w);
                    w = x->parent->left;
                }
                w->red = x->parent->red;
                x->parent->red = 0;
                w->left->red = 0;
                right_rotate(tree, x->parent);
                x = root;
            }
        }
    }
    x->red = 0;
}

void sti_tree_print(sti_tree_t * tree)
{
    sti_print_node(tree, tree->root->left);
}

static void add_to_free(sti_tree_t* tree, sti_node_t* n)
{
    if(tree->free_nodes == 0) {
        tree->free_nodes = n;
    } else {
        n->free_next = tree->free_nodes;
        tree->free_nodes = n;
    }
}

void sti_remove(sti_tree_t * tree, sti_node_t * z)
{
    sti_node_t *y;
    sti_node_t *x;
    sti_node_t *nil = tree->nil;
    sti_node_t *root = tree->root;

    y = ((z->left == nil)
         || (z->right == nil)) ? z : sti_next_node(tree, z);
    x = (y->left == nil) ? y->right : y->left;
    if (root == (x->parent = y->parent)) {
        root->left = x;
    } else {
        if (y == y->parent->left) {
            y->parent->left = x;
        } else {
            y->parent->right = x;
        }
    }
    if (y != z) {
        if (!(y->red)) {
            repair_delete(tree, x);
        }
        y->left = z->left;
        y->right = z->right;
        y->parent = z->parent;
        y->red = z->red;
        z->left->parent = z->right->parent = y;
        if (z == z->parent->left) {
            z->parent->left = y;
        } else {
            z->parent->right = y;
        }
        add_to_free(tree, z);
    } else {
        if (!(y->red)) {
            repair_delete(tree, x);
        }
        add_to_free(tree, y);
    } }

void sti_tree_remove(sti_tree_t * tree, short key)
{
    sti_node_t *x = sti_find_key(tree, key);
    if (x != 0) {
        sti_remove(tree, x);
    }
}

short sti_tree_get_val(sti_tree_t * tree, short key)
{
    sti_node_t *x = sti_find_key(tree, key);
    if (x != 0) {
        return x->info;
    } else {
        return -1;
    }
}

