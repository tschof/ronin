#include "tests.h"
#include "testmacros.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/time.h>

#include "databuf.h"
#include "rex_allocator.h"

static void test_create_dbs(void* g)
{
    databuf_t* node = g;
    node->buffer = g + sizeof(databuf_t);
    node->rd_ptr = node->wr_ptr = node->buffer;
    node->flags = 0x1;
    node->buflen = 512 - sizeof(databuf_t);
}


int test_gatekeeper_databufs(const char** testname, const char** extrareporting)
{
    STARTTEST 
    dallocator* gk = create_mem_glob(512, test_create_dbs);
    REQCOND(gk != 0);
    databuf_t* e = get_stuff(gk);
    databuf_t* f= get_stuff(gk);
    databuf_t* g= get_stuff(gk);
    int i = 0;
    for(;i < 20000; ++i) {
        databuf_t* a = get_stuff(gk);
        CHKCOND(a != 0x0);
    }
    i = 0;
    for(;i < 50000;++i) {
        databuf_t* a = get_stuff(gk);
        databuf_t* b = get_stuff(gk);
        databuf_t* c = get_stuff(gk);
        databuf_t* d = get_stuff(gk);
        CHKCOND(a != 0x0);
        CHKCOND(b != 0x0);
        CHKCOND(c != 0x0);
        CHKCOND(d != 0x0);
        return_stuff(gk, d);
        return_stuff(gk, b);
        return_stuff(gk, a);
        return_stuff(gk, c);
    }
    return_stuff(gk, e);
    return_stuff(gk, f);
    return_stuff(gk, g);
    i = 0;
    for(;i < 50000;++i) {
        databuf_t* d = get_stuff(gk);
        CHKCOND(d != 0x0);
        return_stuff(gk, d);
    }
    destroy_mem_glob(gk);
    STOPTEST
}
