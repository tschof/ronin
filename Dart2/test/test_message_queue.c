
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
int test_queue(const char **testname, const char **extrareporting)
{
    STARTTEST;
    queue_t q = create_queue();
    REQCOND(q != 0);
    CHKCOND(is_empty(q));
    char *dodo = "FUCK";
    char *d2 = "SHIT";
    char *d3 = "COCK_SUCKER";
    enqueue(q, dodo);
    enqueue(q, d2);
    enqueue(q, d3);
    CHKCOND(is_empty(q) == 0);
    element_t e, temp;
    e = dequeue(q);
    CHKCOND(is_empty(q));
    while (e != NULL) {
        CHKCOND(e->d != NULL);
        temp = e;
        e = temp->next;
        free_element(q, temp);
    }
    CHKCOND(is_empty(q));
    int i = 0;
    for (; i < 530; ++i) {
        if (i % 2) {
            enqueue(q, dodo);
        } else {
            enqueue(q, d2);
        }
    }
    e = dequeue(q);
    CHKCOND(is_empty(q));
    i = 0;
    while (e != NULL) {
        CHKCOND(e->d != NULL);
        char* dater = (char*)e->d;
        if (i % 2) {
            CHKCOND(strncmp("FUCK", dater, 4) == 0);
        } else {
            CHKCOND(strncmp("SHIT", dater, 4) == 0);
        }
        temp = e;
        e = temp->next;
        free_element(q, temp);
        ++i;
    }
    CHKCOND(is_empty(q));
    destroy_queue(q);
    STOPTEST;
}

int test_pop_queue(const char **testname, const char **extrareporting)
{
    STARTTEST;
    queue_t q = create_queue();
    element_t e, temp;
    REQCOND(q != 0);
    CHKCOND(is_empty(q));
    int i = 0;
    int j = 0;
    int o = 0;
    for (; i < 20000; ++i) {
        int *t = malloc(sizeof(int));
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
        CHKCOND(j == *((int *) temp->d));
        free(temp->d);
        free_element(q, temp);
        int *u = malloc(sizeof(int));
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
        CHKCOND(j == *((int *) temp->d));
        free(temp->d);
        free_element(q, temp);
        ++j;
    }
    CHKCOND(is_empty(q));
    j = 0;
    i = 0;
    for (; o < 200000; ++o) {
        int *t = malloc(sizeof(int));
        *t = o;
        enqueue(q, t);
    }
    for (; i < 200000; ++i) {
        e = pop_front(q);
        CHKCOND(i == *((int *) e->d));
        free(e->d);
        int *p = malloc(sizeof(int));
        *p = o;
        enqueue(q, p);
        free_element(q, e);
        ++o;
    }
    CHKCOND(!is_empty(q));
    for (; j < 200000; ++j) {
        e = pop_front(q);
        CHKCOND(i == *((int *) e->d));
        free(e->d);
        free_element(q, e);
        ++i;
    }
    CHKCOND(is_empty(q));
    destroy_queue(q);
    STOPTEST;
}
