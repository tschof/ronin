/**************************************************************************
 * Copyright (c) 2008 by Ronin Capital, LLC
 *
 * All Rights Reserved
 *************************************************************************/

#ifndef _DART_CONNECTION_OBJECT_H__
#define _DART_CONNECTION_OBJECT_H__

#include <stddef.h>

#if defined(__cplusplus)
extern "C" {
#endif

    /**
     * This struct is used to map the name of a connection to a
     * particular socket. It also includes the last received sequence
     * numbers from that connection.
     */
    typedef struct dart_con_obj con_obj;

    con_obj *create_empty_con_obj();

    void destroy_con_obj(con_obj * co);

    con_obj *create_con_obj(const char *name, size_t name_len, int sock,
                            int disconnect,
                            unsigned long in_seq, unsigned long out_seq);

    void set_sock(con_obj * co, int sock);

    int get_sock(con_obj * co);

    void set_outgoing_seq(con_obj * co, unsigned long out_seq);

    void set_incoming_seq(con_obj * co, unsigned long in_seq);

    void set_disconnect(con_obj * co, int disconnect);

    unsigned long get_outgoing_seq(con_obj * co);

    int get_is_removed(con_obj * co);

    void set_removed(con_obj * co, int removed);

    void set_should_notify(con_obj * co, int s);

    int get_should_notify(con_obj * co);

    unsigned long get_incoming_seq(con_obj * co);

    int get_disconnect(con_obj * co);

    const char *get_name(con_obj * co);

    void set_name(con_obj * co, const char *name, size_t name_len);

    size_t get_name_len(con_obj * co);

    void increment_incoming_seq(con_obj * co);

    void increment_outgoing_seq(con_obj * co);

#if defined(__cplusplus)
}
#endif
#endif                          //_DART_CONNECTION_OBJECT_H__
