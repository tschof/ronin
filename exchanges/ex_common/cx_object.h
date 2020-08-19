/**************************************************************************
 * Copyright (c) 2008 by Ronin Capital, LLC
 *
 * All Rights Reserved
 *************************************************************************/

#ifndef _CX_OBJECT_H_
#define _CX_OBJECT_H_
#include <arpa/inet.h>
#include "dart_connection_object.h"
#include "client_manager.h"
#include "dart_order_allocator.h"
#include "databuf.h"

#if defined(__cplusplus)
extern "C" {
#endif

#define ntohll(x) ( ( (uint64_t)(ntohl( (uint32_t)((x << 32) >> 32) )) << 32) | ntohl( ((uint32_t)(x >> 32)) ) )
#define htonll(x) ntohll(x)
    typedef struct cx_con_obj cx_obj;

    cx_obj *create_cx_obj(const char *userid, int user_len,
                          const char *password, int pass_len,
                          int session_id, size_t end_hour,
                          size_t end_min, int disconnect,
                          char *mod_name, int mod_name_len, con_obj * obj,
                          void (*send_to_rom) (char *message, int len,
                                               char *rom, int rom_len));
    void destroy_cx_obj(cx_obj * obj);

    void run_cx_admin_loop(cx_obj * obj);
    void set_is_connected(cx_obj * obj, int is_connected);
    int is_cx_connected(cx_obj * obj);
    void kill_cx_connection(cx_obj * obj);

    void set_cx_con_obj(cx_obj * cx, con_obj * co);
    void send_seq_cx_message(cx_obj * obj, char type,
                             dart_ex_order_obj * ord);
    unsigned long frame_cx_mess(cx_obj * obj, const char *mess,
                                unsigned long len, int *cut_con,
                                struct parse_con *pc);


    void add_order_to_maps(cx_obj * obj, dart_ex_order_obj * ord,
                           int cl_ord_id);
    void rom_to_cx_amount(databuf_t * buff, double qty);
    double cx_to_rom_amount(char *data);
    void rom_to_cx_price(databuf_t * buff, char *price, size_t len);
    double cx_to_rom_price(char *data);
    void cx_double_to_char_amount(databuf_t * buff, double amt);
    void cx_double_to_char_price(databuf_t * buff, double price);
    int cx_get_obj_w_rom(cx_obj * c, char *rom_tag, size_t len,
                         void **obj);
    void *cx_get_dart_ord_obj(cx_obj * c);
    char *cx_get_mod_name(cx_obj * c, int *mod_len);
    void add_new_id_to_tree(cx_obj * obj, dart_ex_order_obj * ord,
                            int cl_ord_id);
    dart_ex_order_obj *get_ord_from_tree(cx_obj * obj, int cl_ord);
    void remove_from_tree(cx_obj * obj, int cl_ord);
    void cx_int_destroy(void *a);
    int cx_int_compare(const void *a, const void *b);
#if defined(__cplusplus)
}
#endif
#endif                          //_CX_OBJECT_H_
