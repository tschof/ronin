/**************************************************************************
 * Copyright (c) 2008 by Ronin Capital, LLC
 *
 * All Rights Reserved
 *************************************************************************/
#ifndef _CALL_BACK_WRAPPER_H__
#define _CALL_BACK_WRAPPER_H__
#if defined(__cplusplus)
extern "C" {
#endif

typedef struct call_back_wrapper {
    struct epoll_event *ee;
    int efd;
    int sock;
    void* stuff;
    void (*call_back_func)(void* s);
    void (*clean_up)(void* s);
} call_back_wrapper;
/**
 * Create a call_back_wrapper.  Send in your object as cb_object,
 * this object will be passed back in your cb_func when you are called
 * back.  Pass in a valid socket or a place holder.
 *
 * This function will create and initialize the epoll structure
 * associating the call_back_wrapper with the epoll_event as it's 
 * void* data.ptr.  You can then pass the initialized call_back_wrapper
 * to one of the async_server functions that will add it to the epoll
 * event loop.
 */
call_back_wrapper* create_call_back_wrapper(void* cb_object, 
                                            void (*cb_func)(void* cb_obj),
                                            void (*clean_up)(void* cb_obj),
                                            int socket);

void destroy_call_back_wrapper(call_back_wrapper* cbw);

#if defined(__cplusplus)
}
#endif
#endif //_CALL_BACK_WRAPPER_H__
