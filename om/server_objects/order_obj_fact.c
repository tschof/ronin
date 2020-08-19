/**************************************************************************
 * Copyright (c) 2014 by DART Executions, LLC
 *
 * All Rights Reserved
 *************************************************************************/


#include "order_obj_fact.h"
#include "dart_allocator.h"
#include "databuf.h"

#define DART_POS_LEN 3200


struct order_obj_factory {
    dallocator* d_buffs;
    dallocator* objs;
};

static void init_obj(void* glob)
{
    int lil_obj_size = sizeof(dart_order_obj) + 
                      sizeof(struct db_node) + sizeof(databuf_t);
    int obj_n_node_size = sizeof(dart_order_obj) + sizeof(struct db_node);
    unsigned int buff_offset =lil_obj_size + DART_POS_LEN;
    memset(glob, '\0', DART_PG_SIZE);
    dart_order_obj* doj = glob;
    doj->head = glob + sizeof(dart_order_obj);
    doj->current = doj->head;
    doj->head->start_ptr = glob + buff_offset;
    doj->head->data = glob + obj_n_node_size;
    doj->head->data->buffer =  glob + buff_offset;
    doj->head->data->buflen = DART_PG_SIZE - buff_offset;
    doj->head->end_ptr = glob + DART_PG_SIZE;
    doj->head->data->rd_ptr = doj->head->data->wr_ptr = doj->head->data->buffer;
    doj->head->data->flags = 0x1;
    doj->positions = glob + lil_obj_size;
}
static void init_db(void* g)
{
    struct db_node* node = g;
    node->data = g + sizeof(struct db_node);
    node->data->buffer = g + sizeof(struct db_node) + sizeof(databuf_t);
    node->data->rd_ptr = node->data->wr_ptr = node->data->buffer;
    node->data->flags = 0x1;
    node->data->buflen = 512 - (sizeof(databuf_t) + sizeof(struct db_node));
    node->start_ptr = node->data->buffer;
    node->end_ptr = g + 512;
}

oofact* create_order_obj_factory(unsigned int num_obj)
{
    oofact* ofact = malloc(sizeof(struct order_obj_factory));
    ofact->d_buffs = create_mem_glob(512, init_db);
    ofact->objs = create_mem_glob(num_obj * DART_PG_SIZE, init_obj);
    return ofact;
}

void clean_fact(oofact* ofact)
{
    destroy_mem_glob(ofact->d_buffs);
    destroy_mem_glob(ofact->objs);
    free(ofact);
}
/**
 *
 */
dart_order_obj* get_doj(oofact* ofact)
{
    dart_order_obj* doj = get_stuff(ofact->objs);
    doj->dalloc = ofact;
    return doj;
}

void reset_doj(oofact* ofact, dart_order_obj* doj)
{
    if(doj->childs != 0) {
        children* lch = doj->childs;
        while(lch != NULL) {
            children* llch = lch;
            dart_order_obj* doj_child =(dart_order_obj*)llch->payload;
            lch = lch->next;
            rom_style_reset(doj_child);
            return_stuff(ofact->objs, doj_child);
            free(llch);
        }
        doj->childs = 0;
    }
    return_stuff(ofact->objs, doj);
}


struct db_node* get_new_buffer(oofact* ofact)
{
    void* g = get_stuff(ofact->d_buffs);
    struct db_node* node = g;
    return node;
}
