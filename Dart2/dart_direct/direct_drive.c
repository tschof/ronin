#include "direct_drive.h"
#include "dart_router.h"
#include "dartglob.h"
#include "ex_config_helper.h"
#include "mailer.h"
#include "oats.h"
#include "connection_interface.h"
#include "balance_checker.h"
#include "log_mnger.h"
#include "message_queue.h"
#include "connection_factory.h"
#include "rex_constants.h"
#include "rex_seq_request.h"
#include "risk_cntr_structs.h"

#include <signal.h>
#include <string.h>
#include <time.h>
void dd_glob_log_to_oats(con_int* ab, int is_connected)
{ 
    if(ab->mod_name_len > 0) {
        send_debug_message(ab->o, "Connection: %s, is connected: %d ?\n", ab->mod_name,
                           is_connected);
        if(is_connected) {
            gk_add_connection(ab->gk, ab);
        } 
    }
    if(ab && ab->logon && ab->logon->nlen > 0) {
        set_obj_by_client_id(ab->gk, ab->logon->name, ab->logon->nlen, ab);
        send_debug_message(ab->o, "Connection: %s, is connected: %d ?\n", ab->logon->name,
                           is_connected);
    }
    con_int* oats = get_compliance_con(ab->gk);
    if(oats) {
        //send_oats_update(oats, is_connected, ab->mod_name, ab->mod_name_len, 1, ab);
        if(ab && ab->rc && ab->rc->logon) {
            send_oats_update(oats, is_connected, ab->rc->logon->name, ab->rc->logon->nlen, 1, ab);
        }
    }
}
void dd_glob_log_to_oats_dest(con_int* ab, int is_connected)
{
    if(ab->mod_name_len > 0) {
        send_debug_message(ab->o, "Connection: %s,%s is connected: %d ?\n", ab->mod_name,
                           ab->ex->dest_id, is_connected);
    }
    if(is_connected) {
        gk_add_connection(ab->gk, ab);
    }
    con_int* oats = get_compliance_con(ab->gk);
    if(oats) {
        send_oats_update(oats, is_connected, ab->ex->dest_id, ab->ex->dest_id_len, 0, ab);
    }
}
static void dd_on_fill(con_int* ci, rcntr* cnt)
{
    jp_limits_checker *jlc =
        (jp_limits_checker *) get_balance_struct(ci->gk);
    update_balance_fill(jlc, cnt);
    con_int* sender = 0;
    get_obj_by_client_id(ci->gk, cnt->owner,cnt->olen, (void**)&sender);        
    if(sender) {
        sender->on_update(sender, cnt);
    }
}
static void dd_on_rej(con_int* ci, rcntr* cnt)
{
    jp_limits_checker *jlc =
        (jp_limits_checker *) get_balance_struct(ci->gk);
    update_balance(jlc, cnt);
}
static void dd_on_out(con_int* ci, rcntr* cnt)
{
    jp_limits_checker *jlc =
        (jp_limits_checker *) get_balance_struct(ci->gk);
    update_balance(jlc, cnt);
    con_int* sender = 0;
    get_obj_by_client_id(ci->gk, cnt->owner,cnt->olen, (void**)&sender);        
    if(sender) {
        sender->on_update(sender, cnt);
    }
}
static int dd_on_enter(con_int* ci, rcntr* cnt)
{
    int ret = 1;
    con_int* dest = 0;
    get_destination(ci->gk, 0, 0, (void **) &dest);
    if(dest && !dest->is_blocked && dest->connected) {
        jp_limits_checker *jlc =
            (jp_limits_checker *) get_balance_struct(ci->gk);
        int allowed = update_balance_new(jlc, cnt);
        if (allowed) {
            dest->on_enter_order(dest, cnt);
        } else {
            ret =  -1;
        }
    } else {
        ret =  -1;
    }
    return ret;
}
static int dd_on_replace(con_int* ci,  rcntr* cnt)
{
    con_int* dest = 0;
    get_destination(ci->gk, 0, 0, (void **) &dest);
    jp_limits_checker *jlc =
        (jp_limits_checker *) get_balance_struct(ci->gk);
    int allowed = check_trader_msg_rate(jlc, cnt->owner, cnt->olen, 'E');
    if(allowed && dest && !dest->is_blocked && dest->connected) {
        dest->on_replace_order(dest, cnt);
    } 
    return allowed;
}
static int dd_on_cancel_order(con_int* ci, rcntr* cnt)
{
    con_int* dest = 0;
    get_destination(ci->gk, 0, 0, (void **) &dest);
    jp_limits_checker *jlc =
        (jp_limits_checker *) get_balance_struct(ci->gk);
    int allowed = check_trader_msg_rate(jlc, cnt->owner, cnt->olen, 'E');
    if(dest && dest->connected && allowed) {
        dest->on_cancel_order(dest, cnt);
    }
    return allowed; 
}

struct ddart_control_structs* start_direct_drive(struct dartglob_entry *listeners,
                                                 struct dartglob_entry *globs,
                                                 int argc, char *argv[])
{
    struct ddart_control_structs* dcs =
                            malloc(sizeof(struct ddart_control_structs));
    dcs->interfaces = create_sized_queue(10);
    int flen = strlen(argv[1]);
    dcs->mc = init_config(argv[1], flen);
    dcs->ass = start_base_services(dcs->mc);
    /*char* mds_config = 0x0;
    int md_len = 0;//"mds.dat";
    if(argc >= 5) {
        mds_config = argv[4];
        md_len = strlen(mds_config);
    }*/
    dcs->jp = create_jp_limits_checker(dcs->mc,get_logger(dcs->ass),
                                       dcs->mc->mod_name, dcs->mc->mod_name_len); 
    char* re_routers = "dest_route_tbl.map";
    if(argc >= 6) {
        re_routers = argv[5];
    }
    time_t now;
    time(&now);
    sbrouter* dd = create_dart_router(256,get_logger(dcs->ass), 
                                        dcs->mc->mod_name,
                                        dcs->mc->mod_name_len, re_routers);
    dcs->mc->create_new = ll_before_reset_time(dcs->mc->reset_day,
                                               dcs->mc->reset_hour,
                                               dcs->mc->reset_min,
                                               dcs->mc->last_logon, now);
    dcs->gk = create_dd_gatekeeper(dd,get_logger(dcs->ass),dcs->ass,
                                dcs->mc->mod_name, dcs->mc->mod_name_len,
                                dcs->jp, dcs->mc->create_new);
    dcs->cm = create_gen_life_master(dcs->ass, dcs->mc);
    load_orders(dcs->jp, dcs->gk, dcs->mc->destinations);
    start_dart_mail_service(dcs->mc->mail_server, dcs->mc->mail_server_len,
                            dcs->mc->biz_group, dcs->mc->b_len,
                            dcs->mc->tech_group, dcs->mc->t_len,
                            dcs->mc->end_hour, dcs->mc->end_min,
                            dcs->mc->end_day);
    struct dartglob_entry* glob = listeners;
    for(glob = listeners; glob != 0; glob = glob->next) {
        con_int* lci = create_empty_interface();
        enqueue(dcs->interfaces, lci);
        char* cfile = glob->config_file_name;
        int clen = strlen(glob->config_file_name);
        lci->ex = init_config(cfile, clen);
        lci->mod_name = calloc(lci->ex->ulen + 1, 1);
        memcpy(lci->mod_name, lci->ex->username, lci->ex->ulen);
        lci->mod_name_len = lci->ex->ulen;
        lci->is_server = 1;
        lci->o = get_logger(dcs->ass);
        lci->p = dcs->ass;
        lci->gk = dcs->gk;
        lci->on_enter_order = dd_on_enter;
        lci->on_replace_order = dd_on_replace;
        lci->on_cancel_order = dd_on_cancel_order;
        lci->update_oats = dd_glob_log_to_oats;
        lci->on_cancel_all = 0x0;//cancel_all_orders;
        lci->from_oats_update = 0x0;//cancel_all_orders;
        glob->entrypoint(lci, cfile, clen, dcs->gk);
    }

    for(glob = globs; glob != 0; glob = glob->next) {
        con_int* ldest = create_empty_interface();
        enqueue(dcs->interfaces, ldest);
        char* cfile = glob->config_file_name;
        int clen = strlen(glob->config_file_name);
        ldest->ex = init_config(cfile, clen);
        ldest->mod_name = calloc(ldest->ex->ulen + 1, 1);
        memcpy(ldest->mod_name, ldest->ex->username, ldest->ex->ulen);
        ldest->mod_name_len = ldest->ex->ulen;
        ldest->o = get_logger(dcs->ass);
        ldest->p = dcs->ass;
        ldest->gk = dcs->gk;
        ldest->update_oats = dd_glob_log_to_oats_dest;
        ldest->on_cancel_all = 0x0;//cancel_all_orders;
        ldest->from_oats_update = 0x0;//cancel_all_orders;
        ldest->is_server = 0;
        ldest->on_fill = dd_on_fill;
        ldest->on_rej = dd_on_rej;
        ldest->on_out = dd_on_out;
        glob->entrypoint(ldest, cfile, clen, dcs->gk);
        if(clen == 4 && strncmp("OATS",cfile, 4) == 0) {
            //do nothing this is the oats dest, it will take care of itself.
        } else {
            add_destination(dcs->gk, ldest->ex->dest_id, ldest->ex->dest_id_len,
                            ldest);
        }
    }
    return dcs;
}
