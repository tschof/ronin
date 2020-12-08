/************************************************************************
 *
 *
 * ********************************************************************/

#include "connection_interface.h"
#include "rex_allocator.h"
#include "rex_constants.h"
#include "rex_seq_request.h"
#include "balance_checker.h"

static void cleansing_rinse(struct con_interface* ci){
}
con_int* create_empty_interface()
{
    con_int* ci = 0;
    int ret = 0;
    REX_ALLOC(ci, 8, sizeof(con_int), ret);
    ci->gk = 0;
    ci->cparser = 0;
    ci->ph = 0;
    ci->mod_name = 0;
    ci->mod_name_len = 0;
    ci->should_buffer = 0;
    ci->connected= 0;
    ci->can_send_orders = 0;
    ci->is_blocked = 0;
    ci->sbm = 0;
    ci->tg = 0;
    ci->rc = 0;
    ci->cl_ord_id_map = 0;
    ci->on_enter_order = 0;
    ci->on_replace_order = 0;
    ci->on_cancel_order = 0;
    ci->on_rej = 0;
    ci->on_replaced = 0;
    ci->on_out = 0;
    ci->on_fill = 0;
    ci->on_update = 0;
    ci->on_update = 0;
    ci->reset_seq = 0;
    ci->cleansing_rinse = cleansing_rinse;
    pthread_spin_init(&ci->mutex, PTHREAD_PROCESS_PRIVATE);
    return ci;
}
con_int* init_client_con(char* name, int nlen,
                         gatekeeper* gk)
{
    con_int* ci = 0;
    int ret = 0;
    REX_ALLOC(ci, 8, sizeof(con_int), ret);
    ci->gk = gk;
    ci->cparser = 0;
    ci->ph = 0;
    ci->mod_name = calloc(nlen + 1, 1);
    memcpy(ci->mod_name, name, nlen);
    ci->mod_name_len = nlen;
    ci->should_buffer = 0;
    ci->connected= 0;
    ci->can_send_orders = 0;
    ci->is_blocked = 0;
    ci->sbm = 0;
    ci->tg = 0;
    ci->rc = 0;
    ci->cl_ord_id_map = get_initial_order_map(gk, name, nlen);
    ci->on_enter_order = 0;
    ci->on_replace_order = 0;
    ci->on_cancel_order = 0;
    ci->on_rej = 0;
    ci->on_replaced = 0;
    ci->on_out = 0;
    ci->on_fill = 0;
    ci->on_update = 0;
    ci->on_update = 0;
    ci->reset_seq = 0;
    ci->cleansing_rinse = cleansing_rinse;
    pthread_spin_init(&ci->mutex, PTHREAD_PROCESS_PRIVATE);
    return ci;
}
static void reset_sender(con_int* con)
{
    hashmap_iterator_t hi = hashmap_iterator_begin(con->cl_ord_id_map);
    rcntr* cnt = 0;
    dart_order_obj *buff = 0;
    queue_t q = create_queue();
    element_t e, temp;
    char *key = 0;
    int key_len = 0;
    int valid = hashmap_iterator_dereference(hi, &key,
                                             &key_len,
                                             (void *) &cnt);
    while (valid) {
        enqueue(q, cnt);
        valid = hashmap_iterator_next(con->cl_ord_id_map, hi);
        if (!valid) {
            break;
        }
        valid =
            hashmap_iterator_dereference(hi, &key,
                                         &key_len, (void *) &cnt);
    }
    hashmap_iterator_destroy(hi);

    e = dequeue(q);
    char ord_num[64];
    while (e) {
        cnt = (rcntr*)e->d;
        if(cnt->rlen > 0 && cnt->rlen == sizeof(dart_order_obj)) {
            buff = (dart_order_obj *) cnt->raw;
            memset(ord_num, '\0', 64);
            memcpy(ord_num, buff->positions[ROM_CLORDID].iov_base,
                   buff->positions[ROM_CLORDID].iov_len);
            unsigned long new_id = strtoul(ord_num, NULL, 10);
            buff->sender = con;
            no_copy_insert(con->cl_ord_id_map, (char *) &new_id, 4, buff);
        }
        temp = e;
        e = temp->next;
        temp->d = NULL;
        free_element(q, temp);
    }
    destroy_queue(q);
}
con_int* clone_interface(con_int* cb, dart_order_obj* doj)
{
    con_int* ci = 0;
    int ret = 0;
    REX_ALLOC(ci, 8, sizeof(con_int), ret);
    ci->gk = cb->gk;
    ci->p = cb->p;
    ci->ex = cb->ex;
    ci->o = cb->o;
    ci->rc = cb->rc;
    ci->cparser = cb->cparser;
    ci->ph = cb->ph;
    ci->mod_name = calloc(getplen(doj, ROM_TRADER) + 1, 1);
    memcpy(ci->mod_name, getpval(doj, ROM_TRADER),
           getplen(doj, ROM_TRADER));
    ci->mod_name_len =getplen(doj, ROM_TRADER);
    ci->should_buffer  = cb->should_buffer;
    ci->connected= 0;
    ci->can_send_orders = cb->can_send_orders;
    ci->is_blocked = 0;
    ci->sbm = cb->sbm;
    ci->tg = cb->tg;
    ci->cl_ord_id_map = get_initial_order_map(ci->gk, ci->mod_name,
                                              ci->mod_name_len);
    ci->on_enter_order = cb->on_enter_order;
    ci->on_replace_order = cb->on_replace_order;
    ci->on_cancel_order = cb->on_cancel_order;
    ci->on_rej = cb->on_rej;
    ci->on_replaced = cb->on_replaced;
    ci->on_out = cb->on_out;
    ci->on_fill = cb->on_fill;
    ci->on_update = cb->on_update;
    ci->reset_seq = cb->reset_seq;
    ci->cleansing_rinse = cb->cleansing_rinse;
    ci->on_cancel_all = cb->on_cancel_all;
    ci->send_to_con = cb->send_to_con;
    ci->reload_ip_port = cb->reload_ip_port;
    ci->from_oats_update = cb->from_oats_update; 
    ci->update_oats = cb->update_oats;
    reset_sender(ci);
    pthread_spin_init(&ci->mutex, PTHREAD_PROCESS_PRIVATE);
    return ci;
}
void add_int_orderids(con_int* con)
{
    hashmap_iterator_t hi = hashmap_iterator_begin(con->cl_ord_id_map);
    rcntr* cnt = 0;
    dart_order_obj *buff = 0;
    queue_t q = create_queue();
    element_t e, temp;
    char *key = 0;
    int key_len = 0;
    int valid = hashmap_iterator_dereference(hi, &key,
                                             &key_len,
                                             (void *) &cnt);
    while (valid) {
        enqueue(q, cnt);
        valid = hashmap_iterator_next(con->cl_ord_id_map, hi);
        if (!valid) {
            break;
        }
        valid =
            hashmap_iterator_dereference(hi, &key,
                                         &key_len, (void *) &cnt);
    }
    hashmap_iterator_destroy(hi);

    e = dequeue(q);
    char ord_num[64];
    while (e) {
        cnt = (rcntr*)e->d;
        buff = (dart_order_obj *) cnt->raw;
        memset(ord_num, '\0', 64);
        memcpy(ord_num, buff->positions[ROM_CLORDID].iov_base,
               buff->positions[ROM_CLORDID].iov_len);
        unsigned long new_id = strtoul(ord_num, NULL, 10);
        no_copy_insert(con->cl_ord_id_map, (char *) &new_id, 4, cnt);
        temp = e;
        e = temp->next;
        temp->d = NULL;
        free_element(q, temp);
    }
    destroy_queue(q);
}

login_info* validate_logon(void* rc, dart_order_obj* cur_doj)
{
    con_int* ci = (con_int*)rc;
    char* password = getpval(cur_doj, ROM_PRICE);
    int pLen = getplen(cur_doj, ROM_PRICE);
    char* user = getpval(cur_doj, ROM_TRADER);
    int nLen = getplen(cur_doj, ROM_TRADER);
    return get_logon_struct(get_balance_struct(ci->gk), 
                            password, pLen, 
                            user, nLen);
}

void def_on_connect(con_int* cb, int is_connected)
{
    switch(is_connected) {
        case 0:
            cb->can_send_orders = 0;
            cb->connected = 0;
            break;
        case 1:
            cb->can_send_orders = 1;
            cb->connected = 1;
            break;
        case 2:
            cb->connected = 1;
            cb->can_send_orders = 0;
            break;
    }
}
