/**************************************************************************
 * Copyright (c) 2014 by REX Executions, LLC
 *
 * All Rights Reserved
 *************************************************************************/

#include <pthread.h>

#include "dart_order_fact.h"
#include "rex_allocator.h"
#include "databuf.h"

#define REX_POS_LEN 3200


static struct iovec *def_pos = 0;
static int is_init = 0;

void dart_init_def_pos()
{
    if (!is_init) {
        is_init = 1;
        def_pos = calloc(ROM_LEN, sizeof(struct iovec));
        def_pos[161].iov_base = "\n";
        def_pos[161].iov_len = 1;
        int rom_tag = 0;
        while (rom_tag < 160) {
            ++rom_tag;
            def_pos[rom_tag].iov_base = ",";
            def_pos[rom_tag].iov_len = 1;
            ++rom_tag;
        }
    }
}
void dart_style_reset(dart_order_obj* doj)
{
    dart_init_def_pos();
    memcpy(doj->positions, def_pos, (sizeof(struct iovec) * ROM_LEN));
    doj->in_use = 0;
    doj->ex_mess = 0;
    doj->rep = NULL;
    db_node_t *db = doj->head;
    while (db != NULL) {
        databuf_reset(db->data);
        db = db->next;
    }
    memset(&doj->risk, '\0', sizeof(rcntr));
    doj->risk.raw = doj;
    doj->risk.rlen = sizeof(dart_order_obj);
    doj->current = doj->head;
    doj->positions[161].iov_base = "\n";
    doj->positions[161].iov_len = 1;
    doj->dest = NULL;
    doj->sender = 0;
    doj->childs = 0;
    doj->num_childs = 0;
}
struct dart_order_factory {
    dallocator* d_buffs;
    dallocator* objs;
    pthread_spinlock_t fact_lock;
};

static void init_obj(void* glob)
{
    int lil_obj_size = sizeof(dart_order_obj) + 
                      sizeof(struct db_node) + sizeof(databuf_t);
    int obj_n_node_size = sizeof(dart_order_obj) + sizeof(struct db_node);
    unsigned int buff_offset =lil_obj_size + REX_POS_LEN;
    memset(glob, '\0', REX_PG_SIZE);
    dart_order_obj* doj = glob;
    doj->head = glob + sizeof(dart_order_obj);
    doj->current = doj->head;
    doj->head->start_ptr = glob + buff_offset;
    doj->head->data = glob + obj_n_node_size;
    doj->head->data->buffer =  glob + buff_offset;
    doj->head->data->buflen = REX_PG_SIZE - buff_offset;
    doj->head->end_ptr = glob + REX_PG_SIZE;
    doj->head->data->rd_ptr = doj->head->data->wr_ptr = doj->head->data->buffer;
    doj->head->data->flags = 0x1;
    doj->positions = glob + lil_obj_size;
    dart_style_reset(doj);
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

dartfact* create_dart_order_factory(unsigned int num_obj)
{
    dartfact* ofact = malloc(sizeof(struct dart_order_factory));
    ofact->d_buffs = create_mem_glob(512, init_db);
    ofact->objs = create_mem_glob(num_obj * REX_PG_SIZE, init_obj);
    pthread_spin_init(&ofact->fact_lock, PTHREAD_PROCESS_PRIVATE);
    return ofact;
}

void clean_fact(dartfact* ofact)
{
    destroy_mem_glob(ofact->d_buffs);
    destroy_mem_glob(ofact->objs);
    pthread_spin_destroy(&ofact->fact_lock);
    free(ofact);
}
/**
 *
 */
dart_order_obj* get_doj(dartfact* ofact)
{
    dart_order_obj* doj = 0;
    if (pthread_spin_lock(&ofact->fact_lock) == 0) {
        doj = get_stuff(ofact->objs);
        doj->dalloc = ofact;
        pthread_spin_unlock(&ofact->fact_lock);
    }
    return doj;
}

void reset_doj(dartfact* ofact, dart_order_obj* doj)
{
    if (pthread_spin_lock(&ofact->fact_lock) == 0) {
        if(doj->childs != 0) {
            children* lch = doj->childs;
            while(lch != NULL) {
                children* llch = lch;
                dart_order_obj* doj_child =(dart_order_obj*)llch->payload;
                lch = lch->next;
                dart_style_reset(doj_child);
                return_stuff(ofact->objs, doj_child);
                free(llch);
            }
            doj->childs = 0;
        }
        dart_style_reset(doj);
        return_stuff(ofact->objs, doj);
    pthread_spin_unlock(&ofact->fact_lock);
    }
}


struct db_node* get_new_buffer(dartfact* ofact)
{
    void* g = get_stuff(ofact->d_buffs);
    struct db_node* node = g;
    return node;
}
