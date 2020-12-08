/**************************************************************************
 * Copyright (c) 2014 by DART Executions, LLC
 *
 * All Rights Reserved
 *************************************************************************/
#include <sys/mman.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>

#include "dart_allocator.h"


typedef struct walker_texas_ranger wtr;
#define WTR_SIZE 16
#define WTR_PER_PAGE 256
struct walker_texas_ranger {
    void* payload;
    wtr* next;
};

struct dart_mem_glob {
    wtr* globs;
    wtr* ready;
    wtr* empty;
    void (*init_func)(void *);
    unsigned int size;    
};


unsigned int next_power_of_two(unsigned int size)
{
    int i;
    if (size == 0) {
        return 1;
    }
    --size;

    for (i = 1; i < sizeof(unsigned int) * 8; i <<= 1) {
        size = size | size >> i;
    }
    return size + 1;
}

static void create_keys(dallocator* dmg, wtr* cur_glob)
{
    wtr* keyglob = 0;
    int ret = 0;
    DART_ALLOC(keyglob,8, sizeof(struct walker_texas_ranger), ret);
    keyglob->payload = 0;
    DART_ALLOC(keyglob->payload, DART_PG_SIZE, DART_PG_SIZE, ret);
    unsigned int wtr_step = WTR_SIZE;
    unsigned int total = WTR_PER_PAGE;
    void* offset = keyglob->payload;
    do {
        wtr* cur = offset;
        cur->payload = 0;
        cur->next = dmg->empty;
        dmg->empty = cur;
        total--;
        offset += wtr_step;
    } while(total > 0);
    keyglob->next = cur_glob->next;
    cur_glob->next = keyglob; 
}
static void set_ready_list(dallocator* dmg, wtr* cur_glob)
{
    create_keys(dmg, cur_glob);
    int ret = 0;
    unsigned int total = 0;
    unsigned int step = dmg->size;
    if(dmg->size < DART_PG_SIZE) {
        total = DART_PG_SIZE / dmg->size;
        cur_glob->payload = 0;
        DART_ALLOC(cur_glob->payload, DART_PG_SIZE,DART_PG_SIZE, ret);
    } else {
        cur_glob->payload = 0;
        DART_ALLOC(cur_glob->payload, DART_PG_SIZE, dmg->size, ret);
        total = dmg->size / DART_PG_SIZE;
        step = DART_PG_SIZE;
    }
    void* cur = cur_glob->payload;
    do {
        dmg->init_func(cur);
        return_stuff(dmg, cur);
        cur += step;
        total--;
    } while(total > 0);
}
/**
 * The allocator allocates memory one page
 * at a time and hands it out one "size"
 * chunk at a time.
 */
dallocator* create_mem_glob(unsigned int size,void init_func(void *))
{
    int ret = 0;
    dallocator* dmg = 0;
    DART_ALLOC(dmg, 8, sizeof(struct dart_mem_glob), ret);
    dmg->empty = 0;
    dmg->ready = 0;
    dmg->globs = 0;
    DART_ALLOC(dmg->globs, 8, sizeof(struct walker_texas_ranger), ret);
    dmg->globs->next = 0;
    dmg->size = size;
    dmg->init_func = init_func;
    set_ready_list(dmg, dmg->globs);
    return dmg;
}
/**
 * A mem glob will now return a chunk
 * of memory to you at the size requested
 * when the glob was created.
 */
void* get_stuff(dallocator* dmg)
{
    void* ret = 0;
    if(dmg->ready == 0) {
        wtr* glob1 = 0;
        int ret = 0;
        DART_ALLOC(glob1, 8,sizeof(struct walker_texas_ranger), ret);
        glob1->next = dmg->globs;
        dmg->globs = glob1;
        set_ready_list(dmg, glob1);
    } 
    wtr* cur = dmg->ready;
    dmg->ready = cur->next;
    cur->next = dmg->empty;
    dmg->empty = cur;
    ret =  cur->payload;
    cur->payload = 0;
    return ret;
}
/**
 * In order to avoid unessisary allocations
 * return the stuff after you are done with it.
 */
void return_stuff(dallocator* dmg, void* v)
{
    if(dmg->empty != 0) {
        wtr* cur = dmg->empty;
        dmg->empty = cur->next;
        cur->payload = v;
        cur->next = dmg->ready;
        dmg->ready = cur;
    } else {
        printf("Ran out of empties of size: %u \n", dmg->size);
        create_keys(dmg, dmg->globs);
        wtr* cur = dmg->empty;
        dmg->empty = cur->next;
        cur->payload = v;
        cur->next = dmg->ready;
        dmg->ready = cur;
    }
}
/**
 * Clean up the whole mess, be sure you do not
 * have any pointers to stuff you may have gotten
 * as they will now be invalid.
 */
void destroy_mem_glob(dallocator* dmg)
{ 
    if(dmg->globs) {
        do {
            wtr* cur_glob = dmg->globs;
            dmg->globs = cur_glob->next;
            free(cur_glob->payload);
            free(cur_glob);
        } while(dmg->globs != 0);
    }

    free(dmg);
}


