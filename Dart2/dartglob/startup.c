#include "startup.h"
#include "dartglob.h"
#include "ex_config_helper.h"
#include "mailer.h"
#include "connection_interface.h"
#include "switchboard_master.h"
#include "balance_checker.h"
#include "log_mnger.h"
#include "message_queue.h"
#include "connection_factory.h"
#include "rex_seq_request.h"

#include <signal.h>
#include <string.h>
#include <time.h>

struct dart_control_structs* dartglob_startup(struct dartglob_entry *listeners,
                                              struct dartglob_entry *globs,
                                              int argc, char *argv[])
{
    struct dart_control_structs* dcs =
                            malloc(sizeof(struct dart_control_structs));
    dcs->interfaces = create_sized_queue(10);
    int flen = strlen(argv[1]);
    dcs->mc = init_config(argv[1], flen);
    dcs->ass = start_base_services(dcs->mc);
    char* mds_config = "mds.dat";
    if(argc >= 5) {
        mds_config = argv[4];
    }
    dcs->jp = create_jp_limits_checker(dcs->mc,get_logger(dcs->ass),
                                       mds_config, strlen(mds_config)); 
    char* re_routers = "dest_route_tbl.map";
    if(argc >= 6) {
        re_routers = argv[5];
    }
    time_t now;
    time(&now);
    dcs->mc->create_new = ll_before_reset_time(dcs->mc->reset_day,
                                               dcs->mc->reset_hour,
                                               dcs->mc->reset_min,
                                               dcs->mc->last_logon, now);
    dcs->gk = create_gatekeeper(64,get_logger(dcs->ass),dcs->ass,
                                dcs->mc->mod_name, dcs->mc->mod_name_len,
                                dcs->jp, dcs->mc->create_new, re_routers);
    load_orders(dcs->jp, dcs->gk, dcs->mc->destinations);
    dcs->cm = create_gen_life_master(dcs->ass, dcs->mc);
    start_dart_mail_service(dcs->mc->mail_server, dcs->mc->mail_server_len,
                            dcs->mc->biz_group, dcs->mc->b_len,
                            dcs->mc->tech_group, dcs->mc->t_len,
                            dcs->mc->end_hour, dcs->mc->end_min,
                            dcs->mc->end_day);
    struct dartglob_entry* glob = 0;
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
        lci->on_rej = on_reject;
        lci->on_replaced = on_replaced;
        lci->on_out = on_out;
        lci->on_fill = on_fill;
        lci->on_update = return_to_sender;
        lci->update_oats = glob_log_to_oats;
        lci->on_cancel_all = cancel_all_orders;
        //lci->from_con = switchboard;
    /*Create the parser if it does not exist*/
        glob->entrypoint(lci, cfile, clen, dcs->gk);
    }

    for(glob = globs; glob != 0; glob = glob->next) {
        con_int* lci = create_empty_interface();
        enqueue(dcs->interfaces, lci);
        char* cfile = glob->config_file_name;
        int clen = strlen(glob->config_file_name);
        lci->ex = init_config(cfile, clen);
        lci->mod_name = calloc(lci->ex->ulen + 1, 1);
        memcpy(lci->mod_name, lci->ex->username, lci->ex->ulen);
        lci->mod_name_len = lci->ex->ulen;
        lci->o = get_logger(dcs->ass);
        lci->p = dcs->ass;
        lci->gk = dcs->gk;
        lci->on_rej = on_reject;
        lci->on_replaced = on_replaced;
        lci->on_out = on_out;
        lci->on_fill = on_fill;
        lci->on_update = return_to_sender;
        lci->update_oats = glob_log_to_oats_dest;
        lci->on_cancel_all = cancel_all_orders;
        lci->is_server = 0;
        glob->entrypoint(lci, cfile, clen, dcs->gk);
        add_destination(dcs->gk, lci->ex->dest_id, lci->ex->dest_id_len,
                        lci);
    }
    return dcs;
}
