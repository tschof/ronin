/**************************************************************************
 * Copyright (c) 2008 by Ronin Capital, LLC
 *
 * All Rights Reserved
 *************************************************************************/

#ifndef _EX_COMMON_H__
#define _EX_COMMON_H__

#if defined(__cplusplus)
extern "C" {
#endif

#include "message_queue.h"
#include "client_manager.h"
#include "fix_rom_trans.h"
#include "dart_orderbook.h"
#include "dart_order_allocator.h"
#include "config_loader.h"


    void exchange_async_write_callback(const char *name, int name_len,
                                       void *cb);
    void exchange_async_rom_sender(struct message *mess, orderbook * ob);
    void async_iovec_sender(struct iovec *iov, int iovcnt, char *name,
                            int name_len, orderbook * ob, int sock);

    size_t build_cnl_rpl_rej(char *beg_off, size_t len,
                             char *l_fix_tag,
                             databuf_t * output, const char *message);

    void set_val_in_order_struct(char *value, size_t len,
                                 int offset,
                                 dart_ex_order_obj * obj, char *type);

    int set_execution_time(databuf_t * d_off);

    int set_rom_val_in_fix(databuf_t * d_off, char *b_off, struct dart_fix_pos
                           positions[ROM_LEN], int offset,
                           char *type,
                           dart_ex_order_obj * obj,
                           void (*set_val_in_order_struct)
                            (char *b, size_t len, int off,
                             dart_ex_order_obj * obj, char *type));

    size_t translate_rom_symbol(databuf_t * d_off,
                                char *symbol, size_t sym_len);

    long send_alert_for_message(struct message *mess,
                                char *fix_message,
                                databuf_t * output,
                                long fix_len,
                                char rom_type, struct dart_rom_pos
                                positions[ROM_LEN]);

    void set_replace_delta(char *value, size_t len,
                           dart_ex_order_obj * obj);

    void set_shares_fields(char *value, size_t len,
                           dart_ex_order_obj * obj);

    void update_shares_for_replace(char *value, size_t len,
                                   dart_ex_order_obj * obj);

    void update_shares_for_fill(char *value, size_t len,
                                dart_ex_order_obj * obj);

    void send_message_to_roms(struct message *mess, orderbook * ob);

    size_t create_rom_reject(char *m,
                             databuf_t * output, const char *message);

    void load_rom_config(dart_config * dc, queue_t rom_queue);

    struct init_args *load_fix_config(const char *filename, int len);

    void free_init_args(struct init_args *init);

    char *build_rom_name(const char *om_tag,
                         size_t om_tag_len, size_t * r_name_len);

    size_t add_rom_id_to_mess(databuf_t * out_mess,
                              dart_ex_order_obj * obj);

    size_t clean_text(databuf_t * out_mess,
                      const char *fix_mess, size_t fix_len,
                      int offset, struct dart_rom_pos positions[]);

    size_t handle_clordids(char *status,
                           databuf_t * out_mess,
                           const char *fix_mess,
                           struct dart_rom_pos positions[],
                           pthread_mutex_t * map_mutex,
                           hashmap_t cl_ord_id_map,
                           dart_ex_order_obj * obj);

    void check_n_clean(char *status, hashmap_t cl_ord_id,
                       hashmap_t rom_id_map,
                       dart_ex_order_obj * obj,
                       pthread_mutex_t * map_mutex,
                       char rom_type, orderbook * ob,
                       char *fix_message, int fix_len,
                       struct dart_rom_pos positions[]);
    int get_order_object(char *fix_message, size_t fix_len,
                         hashmap_t cl_ord_id_map,
                         struct dart_rom_pos positions[],
                         pthread_mutex_t * map_mut,
                         char rom_type,
                         dart_ex_order_obj ** obj,
                         struct message *mess,
                         databuf_t * output, orderbook * ob);

    long set_status_for_cnrej(char *fix_message, struct dart_rom_pos
                              positions[], char *status,
                              databuf_t * output, dart_ex_order_obj* obj);

    dart_ex_order_obj *establish_order(char *fix_message,
                                       pthread_mutex_t * mut,
                                       hashmap_t cl_map,
                                       hashmap_t rom_map,
                                       struct dart_fix_pos
                                       positions[],
                                       struct message *mess,
                                       char *cl_ord_id,
                                       size_t cl_len, orderbook * ob);

    struct database_conf;
    void database_init(struct database_conf *, const char *destid,
                       queue_t q, hashmap_t cl_ord_id_map,
                       hashmap_t rom_ord_id_map, orderbook * ob);

    int check_exe_inst(struct dart_fix_pos positions[],
                       const char *rom_message);

    void from_fix_func(struct message *mess, void *ob);
    int def_fixrom_status_xlate(char *fixmsg,
                                dart_rom_pos * xlateinfo,
                                char mtype, char *status,
                                databuf_t * rom,
                                size_t romfield,
                                int *rejtected,
                                dart_ex_order_obj * order,
                                pthread_mutex_t * mutex,
                                hashmap_t ord_map);
    int def_add_rom_tag(char *fixmsg,
                        dart_rom_pos * xlateinfo, char mtype,
                        char *status, databuf_t * rom,
                        size_t romfield, int *rejtected,
                        dart_ex_order_obj * order,
                        pthread_mutex_t * mutex, hashmap_t ord_map);
    int def_xlate_fixrom(char *fixmsg,
                         dart_rom_pos * xlateinfo,
                         char mtype, char *status,
                         databuf_t * rom, size_t romfield,
                         int *rejtected,
                         dart_ex_order_obj * order,
                         pthread_mutex_t * mutex, hashmap_t ord_map);
    int def_no_op(char *fixmsg, dart_rom_pos * xlateinfo,
                  char mtype, char *status, databuf_t * rom,
                  size_t romfield, int *rejtected,
                  dart_ex_order_obj * order,
                  pthread_mutex_t * mutex, hashmap_t ord_map);
    int def_handle_ordids(char *fixmsg,
                          dart_rom_pos * xlateinfo,
                          char mtype, char *status,
                          databuf_t * rom, size_t romfield,
                          int *rejtected,
                          dart_ex_order_obj * order,
                          pthread_mutex_t * mutex, hashmap_t ord_map);

    /** \brief Used for any text field mapping that may have embedded
     *  commas.  \return Successful completion returns the number of
     *  characters written to the ROM message, which could be zero,
     *  otherwise returning -1 to indicate failure.
     *
     *  Any free-form text field coming from FIX must be purged of all
     *  commas, to not do so would corrupt the comma-separated ROM
     *  message we are generating.
     */
    int def_map_fix_text_to_rom(char *fixmsg,
                                dart_rom_pos * positions,
                                char mtype, char *status,
                                databuf_t * rom,
                                size_t romfield,
                                int *rejtected,
                                dart_ex_order_obj * order,
                                pthread_mutex_t * mutex,
                                hashmap_t ord_map);

    long def_fix_to_rom_override(struct message *mess,
                                 char *fix_message,
                                 databuf_t * output,
                                 long fix_len,
                                 char rom_type, struct dart_rom_pos
                                 positions[ROM_LEN],
                                 int *rejected, orderbook * ob);

    int update_spread_head(dart_ex_order_obj * obj, char *status, int len,
                           char rom_type, const char *fix_mess,
                           int fix_len, struct dart_rom_pos positions[],
                           pthread_mutex_t * map_mutex,
                           hashmap_t cl_ord_id_map, databuf_t * output);
    int gen_build_clear_id_map(const char *filename, hashmap_t map);
#if defined(__cplusplus)
}
#endif
#endif                          //_MAILER_H__
