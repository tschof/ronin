/**************************************************************************
 * Copyright (c) 2008 by Ronin Capital, LLC
 *
 * All Rights Reserved
 *************************************************************************/
#include <string.h>
#include <stdlib.h>
#include <sys/epoll.h>

#include "call_back_wrapper.h"



call_back_wrapper* create_call_back_wrapper(void* cb_object, 
                                            void* (*cb_func)(void* cb_obj),
                                            void (*clean_up)(void* cb_obj),
                                            int socket)
{
    call_back_wrapper* cbw = calloc(1, sizeof(struct call_back_wrapper));
    cbw->stuff = cb_object;
    cbw->call_back_func = cb_func;
    cbw->ee = calloc(1, sizeof(struct epoll_event));
    cbw->sock = socket;
    cbw->ee->data.ptr = cbw;
    cbw->clean_up = clean_up;
    return cbw;
}
void destroy_call_back_wrapper(call_back_wrapper* cbw)
{
    if(cbw && cbw->ee) {
        free(cbw->ee);
        cbw->ee = 0;
        free(cbw);
        cbw = 0;
    }
}
