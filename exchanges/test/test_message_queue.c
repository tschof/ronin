
#include "tests.h"
#include "testmacros.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/time.h>


#include "message_queue.h"
#include "dart_order_allocator.h"
#include "order_token_creator.h"
int test_queue(const char **testname, const char **extrareporting)
{
    STARTTEST queue_t q = create_queue();
    queue_t fr = get_free_list(q);
    REQCOND(q != 0);
    CHKCOND(is_empty(q));
    char *dodo = "FUCK";
    char *d2 = "SHIT";
    char *d3 = "COCK_SUCKER";
    enqueue(q, dodo);
    enqueue(q, d2);
    enqueue(q, d3);
    CHKCOND(is_empty(q) == 0);
    int j = 0;
    element_t e, temp;
    e = dequeue(q);
    CHKCOND(is_empty(q));
    while (e != NULL) {
        CHKCOND(e->d != NULL);
        temp = e;
        e = temp->next;
        free_element(q, temp);
        element_t l = front(fr);
        if (j == 0) {
            CHKCOND(l->next->d == NULL);
            CHKCOND(l->next->next->d == NULL);
        } else if (j == 1) {
            CHKCOND((l->next->d == NULL));
            CHKCOND((l->next->next->d == NULL));
        } else {
            CHKCOND(l->next->d == NULL);
            CHKCOND(l->next->next->d == NULL);
            CHKCOND(l->next->next->next->d == NULL);
        }
        ++j;
    }
    CHKCOND(is_empty(q));
    int i = 0;
    for (; i < 1000; ++i) {
        if (i % 2) {
            enqueue(q, dodo);
        } else {
            enqueue(q, d2);
        }
    }
    CHKCOND(is_empty(fr));
    enqueue(q, d2);
    ++i;
    CHKCOND(!is_empty(fr));
    for (; i < 3000; ++i) {
        if (i % 2) {
            enqueue(q, dodo);
        } else {
            enqueue(q, d2);
        }
    }
    CHKCOND(is_empty(fr));
    enqueue(q, d2);
    enqueue(q, dodo);
    CHKCOND(!is_empty(fr));
    e = dequeue(q);
    CHKCOND(is_empty(q));
    i = 0;
    while (e != NULL) {
        CHKCOND(e->d != NULL);
        temp = e;
        e = temp->next;
        free_element(q, temp);
        element_t l = front(fr);
        if (i % 2) {
            CHKCOND(l->next->d == NULL);
        } else {
            CHKCOND(l->next->d == NULL);
        }
        ++i;
    }
    CHKCOND(is_empty(q));
STOPTEST}

static token_struct_t *idgenerator = 0;
static char datestr[9] = { '\0' };
static int initialize_date_string(void)
{
    int retval = -1;
    time_t now = time(&now);
    struct tm *now_tm = gmtime(&now);
    if (now_tm != 0) {
        int nwritten = sprintf(datestr, "%04d%02d%02d",
                               now_tm->tm_year + 1900,
                               now_tm->tm_mon + 1,
                               now_tm->tm_mday);
        if (nwritten <= sizeof datestr) {
            retval = 0;
        }
    }
    return retval;
}
static char *arca_fix_generate_id(size_t * nlen)
{
    char *cl_ord_id = 0;
    if (nlen != 0) {
        int retval = 0;
        cl_ord_id = calloc(DART_EX_TAG_LEN, 1);
        memcpy(cl_ord_id, datestr, sizeof datestr - 1);
        if ((retval = create_order_token(idgenerator, cl_ord_id +
                                         (sizeof datestr - 1),
                                         DART_EX_TAG_LEN - 1)) > 0) {
            *nlen = (size_t) retval + (sizeof datestr - 1);
        } else {
            *nlen = 0;
        }
    }
    return cl_ord_id;
}

void* sideways_with_a_barge_poll(void* d)
{
    queue_t q = create_queue(2);
    element_t e, temp;
    int i = 0;
    char* prev = 0;
    char* cur = 0;
    size_t len = 0;
    for(; i < 500000; ++i) {
        cur = arca_fix_generate_id(&len);
        if(strlen(cur) != (int)len) {
            printf("We are FUCKED! %lu, %lu \n", strlen(cur), len);
        }
        if(i > 0) {
            if(strncmp(cur, prev, len) == 0) {
                printf("We are FUCKED! %s, %s\n", cur, prev);
            }
        }
        prev = cur;
        enqueue(q, cur);
    }
    e = dequeue(q);
    i = 0;
    while (e != NULL) {
        cur = (char*)e->d;
        if(i != 0) {
            if(strncmp(cur, prev, (strlen(cur))) == 0) {
                printf("We are FUCKED! %s, %s\n", cur, prev);
            }
            free(prev);
            prev = 0;
        }
        prev = cur; 
        temp = e;
        e = temp->next;
        free_element(q, temp);
        ++i;
    }
    return NULL;
}
int test_token_up(const char **testname, const char **extrareporting)
{
    STARTTEST queue_t q = create_queue(2);
    REQCOND(q != 0);
    CHKCOND(is_empty(q));
    void *result;
    initialize_date_string();
    idgenerator = init_order_token_creator();
    pthread_t num1;
    pthread_t num2;
    int status = pthread_create(&num1, NULL, &sideways_with_a_barge_poll, q);
    status = pthread_create(&num2, NULL, &sideways_with_a_barge_poll, q);
    status = pthread_join(num1, &result);
    status = pthread_join(num2, &result);
    CHKCOND(is_empty(q));
STOPTEST}


int test_allocate_master(const char **testname, const char **extrareporting)
{
    STARTTEST queue_t q = create_queue(2);
    element_t e, temp;
    REQCOND(q != 0);
    CHKCOND(is_empty(q));


    dart_obj_manager * dm = startup_allocator(5);

    void*  last_offset = 0;
    void*  prev_delete = 0;
    dart_ex_order_obj * lobj = 0;
    int i = 0;
    for(; i < 50000; ++i) {
        lobj = get_empty_dart_order_obj(dm);
        if(i > 0) {
            CHKCOND(lobj != last_offset);
        } 
        last_offset = lobj;
        destroy_dart_order_obj(dm, lobj);
    }
    i = 0;
    for(; i < 200000; ++i) {
        lobj = get_empty_dart_order_obj(dm);
        if(i > 0) {
            CHKCOND(lobj != last_offset);
        } 
        last_offset = lobj;
        enqueue(q, lobj);
    }

    e = dequeue(q);
    CHKCOND(is_empty(q));
    i = 0;
    while (e != NULL) {
        CHKCOND(e->d != NULL);
        lobj = (dart_ex_order_obj*)e->d;
        if(i > 0) {
            CHKCOND(lobj != prev_delete);
        }
        temp = e;
        e = temp->next;
        prev_delete = lobj;
        destroy_dart_order_obj(dm, lobj);
        free_element(q, temp);
        ++i;
    }
    i = 0;
    for(; i < 50000; ++i) {
        lobj = get_empty_dart_order_obj(dm);
        if(i > 0) {
            CHKCOND(lobj != last_offset);
        } 
        last_offset = lobj;
        destroy_dart_order_obj(dm, lobj);
    }
    CHKCOND(is_empty(q));
STOPTEST}

int test_pop_queue(const char **testname, const char **extrareporting)
{
    STARTTEST queue_t q = create_queue(2);
    element_t e, temp;
    REQCOND(q != 0);
    CHKCOND(is_empty(q));
    int i =0;
    int j = 0;
    int o = 0;
    for(; i < 20000; ++i) {
        int* t = malloc(sizeof(int));
        *t = i;
        enqueue(q, t);
    }
    CHKCOND(!is_empty(q));
    e = dequeue(q);
    CHKCOND(is_empty(q));
    while (e != NULL) {
        CHKCOND(e->d != NULL);
        temp = e;
        e = temp->next;
        CHKCOND(j == *((int*)temp->d));
        free(temp->d);
        free_element(q, temp);
        int* u = malloc(sizeof(int));
        *u = i;
        enqueue(q, u);
        ++i;
        ++j;
    }
    CHKCOND(!is_empty(q));
   
    e = dequeue(q);
    CHKCOND(is_empty(q));
    while (e != NULL) {
        CHKCOND(e->d != NULL);
        temp = e;
        e = temp->next;
        CHKCOND(j == *((int*)temp->d));
        free(temp->d);
        free_element(q, temp);
        ++j;
    }
    CHKCOND(is_empty(q));
    j = 0;
    i = 0;
    for(; o < 200000; ++o) {
        int* t = malloc(sizeof(int));
        *t = o;
        enqueue(q, t);
    }
    for(; i < 200000; ++i) {
        e = pop_front(q);
        CHKCOND(i == *((int*)e->d));
        free(e->d);
        free_element(q, e);
        int* p = malloc(sizeof(int));
        *p = o;
        enqueue(q, p);
        ++o;
    }
    CHKCOND(!is_empty(q));
    for(; j < 200000; ++j) {
        e = pop_front(q);
        CHKCOND(i == *((int*)e->d));
        free(e->d);
        free_element(q,e);
        ++i;
    } 
    CHKCOND(is_empty(q));
    destroy_queue(q);
STOPTEST}
