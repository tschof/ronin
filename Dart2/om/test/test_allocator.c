#include "tests.h"
#include "testmacros.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/time.h>

#include "dart_allocator.h"
#include "gatekeeper.h"


int test_gatekeeper_databufs(const char** testname, const char** extrareporting)
{
    STARTTEST 
    gatekeeper *gk = create_empty_gatekeeper(8,
                              "DODO", 4,
                              0, 0);
    REQCOND(gk != 0);
    databuf_t* e = get_databuf(gk);
    databuf_t* f= get_databuf(gk);
    databuf_t* g= get_databuf(gk);
    int i = 0;
    for(;i < 20000; ++i) {
        databuf_t* a = get_databuf(gk);
        CHKCOND(a != 0x0);
    }
    i = 0;
    for(;i < 50000;++i) {
        databuf_t* a = get_databuf(gk);
        databuf_t* b = get_databuf(gk);
        databuf_t* c = get_databuf(gk);
        databuf_t* d = get_databuf(gk);
        CHKCOND(a != 0x0);
        CHKCOND(b != 0x0);
        CHKCOND(c != 0x0);
        CHKCOND(d != 0x0);
        destroy_gk_db(gk, d);
        destroy_gk_db(gk, b);
        destroy_gk_db(gk, a);
        destroy_gk_db(gk, c);
    }
    destroy_gk_db(gk, e);
    destroy_gk_db(gk, f);
    destroy_gk_db(gk, g);
    i = 0;
    for(;i < 50000;++i) {
        databuf_t* d = get_databuf(gk);
        CHKCOND(d != 0x0);
        destroy_gk_db(gk, d);
    }
    clean_obj(gk);
    STOPTEST
}
