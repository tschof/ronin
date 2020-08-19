/**************************************************************************
 * Copyright (c) 2008 by Ronin Capital, LLC
 *
 * All Rights Reserved
 *************************************************************************/
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "rb_tree.h"

struct rb_node {
    void *key;
    void *info;
    int red;
    int delete_val;
    struct rb_node *left;
    struct rb_node *right;
    struct rb_node *parent;
};

struct rb_tree {
    int (*rb_compare) (const void *, const void *);
    void (*rb_destroy_key) (void *);
    void (*rb_destroy_info) (void *);
    void (*print_func_key) (const void *);
    void (*print_func_info) (const void *);
    rb_node_t *root;
    rb_node_t *nil;
};

/*@null@*/
rb_tree_t *create_rb_tree(int (*compare_func) (const void *, const void *),
                          void (*destroy_func) (void *),
                          void (*destroy_info_func) (void *),
                          void (*print_func) (const void *),
                          void (*print_func_info) (const void *))
{
    rb_tree_t *ret_tree = malloc(sizeof(struct rb_tree));
    rb_node_t *temp;
    if (ret_tree) {
        ret_tree->rb_compare = compare_func;
        ret_tree->rb_destroy_key = destroy_func;
        ret_tree->rb_destroy_info = destroy_info_func;
        ret_tree->print_func_key = print_func;
        ret_tree->print_func_info = print_func_info;
        ret_tree->nil = malloc(sizeof(struct rb_node));
        if (ret_tree->nil) {
            temp = ret_tree->nil;
            temp->parent = temp->left = temp->right = temp;
            temp->red = 0;
            temp->key = 0;
            temp->delete_val = 0;
            temp = ret_tree->root = malloc(sizeof(struct rb_node));
            if (temp) {
                temp->parent = temp->left = temp->right = ret_tree->nil;
                temp->key = 0;
                temp->red = 0;
                temp->delete_val = 0;
            } else {
                free(ret_tree->nil);
                ret_tree->nil = 0;
                free(ret_tree);
                ret_tree = 0;
            }
        } else {
            free(ret_tree);
            ret_tree = 0;
        }
    }
    return ret_tree;
}

void left_rotate(rb_tree_t * tree, rb_node_t * x)
{
    rb_node_t *y;
    rb_node_t *nil = tree->nil;

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

void right_rotate(rb_tree_t * tree, rb_node_t * y)
{
    rb_node_t *x;
    rb_node_t *nil = tree->nil;

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

void tree_insert_help(rb_tree_t * tree, rb_node_t * z)
{
    rb_node_t *x;
    rb_node_t *y;
    rb_node_t *nil = tree->nil;
    z->left = z->right = nil;
    y = tree->root;
    x = tree->root->left;
    while (x != nil) {
        y = x;
        if (1 == tree->rb_compare(x->key, z->key)) {
            x = x->left;
        } else {
            x = x->right;
        }
    }
    z->parent = y;
    if ((y == tree->root) || (1 == tree->rb_compare(y->key, z->key))) {
        y->left = z;
    } else {
        y->right = z;
    }
}

static rb_node_t *rb_tree_insert2(rb_tree_t * tree, void *key, void *val,
                                  int delete)
{
    rb_node_t *y;
    rb_node_t *x;
    rb_node_t *new_node;

    x = malloc(sizeof(struct rb_node));
    x->key = key;
    x->info = val;
    x->delete_val = delete;
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

rb_node_t *rb_tree_no_delete_insert(rb_tree_t * tree, void *key, void *val)
{
    return rb_tree_insert2(tree, key, val, 0);
}

rb_node_t *rb_tree_insert(rb_tree_t * tree, void *key, void *val)
{
    return rb_tree_insert2(tree, key, val, 1);
}

rb_node_t *tree_next_node(rb_tree_t * tree, rb_node_t * x)
{
    rb_node_t *y;
    rb_node_t *nil = tree->nil;
    rb_node_t *root = tree->root;

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

rb_node_t *tree_prev_node(rb_tree_t * tree, rb_node_t * x)
{
    rb_node_t *y;
    rb_node_t *nil = tree->nil;
    rb_node_t *root = tree->root;

    if (nil != (y = x->left)) {
        while (y->right != nil) {
            y = y->right;
        }
        return y;
    } else {
        y = x->parent;
        while (x == y->left) {
            if (y == root) {
                return nil;
            }
            x = y;
            y = y->parent;
        }
        return y;
    }
}

void kill_node(rb_tree_t * tree, rb_node_t * x)
{
    rb_node_t *nil = tree->nil;
    if (x != nil) {
        kill_node(tree, x->left);
        kill_node(tree, x->right);
        tree->rb_destroy_key(x->key);
        if (x->delete_val) {
            tree->rb_destroy_info(x->info);
        }
        free(x);
    }
}

void print_node(rb_tree_t * tree, rb_node_t * x)
{
    rb_node_t *nil = tree->nil;
    rb_node_t *root = tree->root;
    if (x != tree->nil) {
        print_node(tree, x->left);
        printf("\nInfo= ");
        tree->print_func_info(x->info);
        printf("  Key= ");
        tree->print_func_key(x->key);
        printf("  l->key = ");
        if (x->left == nil) {
            printf("NULL");
        } else {
            tree->print_func_key(x->left->key);
        }
        printf(" r->key= ");
        if (x->right == nil) {
            printf("NULL");
        } else {
            tree->print_func_key(x->right->key);
        }
        printf(" p->key= ");
        if (x->parent == root) {
            printf("ROOT_NULL");
        } else {
            tree->print_func_key(x->parent->key);
        }
        printf(" red=%i\n", x->red);
        print_node(tree, x->right);
    }
}

rb_node_t *rb_find_key(rb_tree_t * tree, void *q)
{
    rb_node_t *x = tree->root->left;
    rb_node_t *nil = tree->nil;
    int comp_val;
    if (x == nil) {
        return 0;
    }
    comp_val = tree->rb_compare(x->key, (int *) q);
    while (0 != comp_val) {
        if (1 == comp_val) {
            x = x->left;
        } else {
            x = x->right;
        }
        if (x == nil) {
            return 0;
        }
        comp_val = tree->rb_compare(x->key, (int *) q);
    }
    return x;
}

void repair_delete(rb_tree_t * tree, rb_node_t * x)
{
    rb_node_t *root = tree->root->left;
    rb_node_t *w;
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

void rb_tree_print(rb_tree_t * tree)
{
    print_node(tree, tree->root->left);
}

void rb_remove(rb_tree_t * tree, rb_node_t * z)
{
    rb_node_t *y;
    rb_node_t *x;
    rb_node_t *nil = tree->nil;
    rb_node_t *root = tree->root;

    y = ((z->left == nil)
         || (z->right == nil)) ? z : tree_next_node(tree, z);
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
        free(z);
    } else {
        if (!(y->red)) {
            repair_delete(tree, x);
        }
        free(y);
    }
}

void rb_delete(rb_tree_t * tree, rb_node_t * z)
{
    rb_node_t *y;
    rb_node_t *x;
    rb_node_t *nil = tree->nil;
    rb_node_t *root = tree->root;

    y = ((z->left == nil)
         || (z->right == nil)) ? z : tree_next_node(tree, z);
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
        tree->rb_destroy_key(z->key);
        if (z->delete_val) {
            tree->rb_destroy_info(z->info);
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
        free(z);
    } else {
        tree->rb_destroy_key(y->key);
        if (y->delete_val) {
            tree->rb_destroy_info(y->info);
        }
        if (!(y->red)) {
            repair_delete(tree, x);
        }
        free(y);
    }
}

void rb_tree_remove(rb_tree_t * tree, void *key)
{
    rb_node_t *x = rb_find_key(tree, key);
    if (x != 0) {
        rb_remove(tree, x);
    }
}

void rb_tree_delete(rb_tree_t * tree, void *key)
{
    rb_node_t *x = rb_find_key(tree, key);
    if (x != 0) {
        rb_delete(tree, x);
    }
}

void rb_tree_destroy(rb_tree_t * tree)
{
    kill_node(tree, tree->root->left);
    free(tree->root);
    free(tree->nil);
    free(tree);
}

void *rb_tree_get_val(rb_tree_t * tree, void *key)
{
    rb_node_t *x = rb_find_key(tree, key);
    if (x != 0) {
        return x->info;
    } else {
        return 0;
    }

}
