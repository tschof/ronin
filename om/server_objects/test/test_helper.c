#include "test_helper.h"
#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <stdint.h>

void dump_current_thread_id(void)
{
    pthread_t self = pthread_self();
    size_t i;
    const char *separator = "";
    for (i = 0; i < sizeof self; ++i) {
        if (i != 0) {
            separator = " ";
        }
        uint byte = ((uint8_t *) self)[i];
        printf("%s%02x", separator, byte);
    }
}
