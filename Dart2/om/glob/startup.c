#include "startup.h"
#include "dartglob.h"
#include "ex_config_helper.h"
#include "mailer.h"
#include "rom_handler.h"
#include "connection_interface.h"
#include "switchboard_master.h"
#include "master_config.h"
#include "jigglypuff.h"
#include "oats.h"
#include "debug_logger.h"
#include "lifecycle_master.h"

#include <signal.h>
#include <string.h>

void* reconnect(void *ci);
void* block(void* ci);

int glob_jiggly_wrapper(void *j, dart_order_obj * doj)
{
    return 0;
}

static int *set_cpus(char *cfg_cpu, int cfg_cpu_len, int *ncpus)
{
    int i = 1;
    char *off = cfg_cpu;
    int cur_len = cfg_cpu_len;
    while ((off = memchr(off, ',', cur_len)) != NULL) {
        ++i;
        off += 1;
        cur_len = cfg_cpu_len - (off - cfg_cpu);
    }
    *ncpus = i;
    int *cpus = calloc(i * sizeof(int), 1);
    char *cpu_tok = cfg_cpu;
    char *c = memchr(cpu_tok, ',', cfg_cpu_len);
    int j = 0;
    cur_len = cfg_cpu_len;
    while (c) {
        cpus[j] = atoi(cpu_tok);
        cpu_tok = c + 1;
        cur_len = cfg_cpu_len - (cpu_tok - cfg_cpu);
        ++j;
        c = memchr(cpu_tok, ',', cur_len);
    }
    if (j < i) {
        cpus[j] = atoi(cpu_tok);
    }
    return cpus;
}

struct dart_control_structs* dartglob_startup(struct dartglob_entry *globlist, int argc,
                     char *argv[])
{
    struct dart_control_structs* dcs = malloc(sizeof(struct dart_control_structs));
    dcs->interfaces = create_sized_queue(10);
    rec_args* ra = init_recorder(64, 32, "./",2);
    int len = strlen(argv[1]);
    master_config *main_cfg = init_mconfig(argv[1], len);
    dcs->mc = main_cfg;
    if(main_cfg->dir_len > 0) {
        reset_default_directory(ra, main_cfg->dir, main_cfg->dir_len);
    }
    int ncpus = 0;
    int *cpus = set_cpus(main_cfg->cpus, main_cfg->cpu_len, &ncpus);
    if(ncpus < 2) {
        return NULL;
    }
    jp_limits_checker *jlc =0;
    if(argc >= 6) {
        char* mdsfile = argv[5];
        jlc = create_jp_limits_checker(main_cfg, mdsfile, strlen(mdsfile));
    } else {
        jlc = create_jp_limits_checker(main_cfg, NULL, 0);
    }
    dcs->jp = jlc;
    queue_t users = get_allowed_logons(jlc);
    char *re_routers = "dest_route_tbl.map";
    if (argc >= 5) {
        re_routers = argv[4];
    }
    gatekeeper *gk = create_gatekeeper(8192, (char *) main_cfg->mod_name,
                                       main_cfg->mod_name_len,
                                       jlc, main_cfg->create_new,
                                       re_routers, glob_jiggly_wrapper);
    dcs->gk = gk;
    async_server_con_set * async_server =
        start_async_server(gk, ra,
                           ncpus, 0,cpus, 64);
    dcs->ass = async_server;
    dcs->cm = create_lifecycle_master(async_server, gk, main_cfg);
    /* i.e. processing the first in our list */
    start_dart_mail_service(main_cfg->mail_server,
                            main_cfg->mail_server_len,
                            main_cfg->biz_group,
                            main_cfg->b_len,
                            main_cfg->tech_group,
                            main_cfg->t_len,
                            main_cfg->end_hour,
                            main_cfg->end_min, main_cfg->end_day);
    load_orders(jlc, gk, main_cfg->destinations);
        add_async_listener(async_server, gk, main_cfg, main_cfg->mod_name,
                           main_cfg->mod_name_len,
                           main_cfg->server_port,
                           users,
                           main_cfg->server_ip,
                           main_cfg->server_ip_len,
                           main_cfg->end_hour,
                           main_cfg->end_min,
                           main_cfg->end_day,
                           0,
                           glob_on_connect,
                           ph_parse_rom_message,
                           0,
                           ci_async_write_callback,
                           ph_build_non_fix_rom_pos, switchboard);
        if(main_cfg->server_ip_len > 0) {
            add_async_listener(async_server, gk, main_cfg, main_cfg->mod_name,
                               main_cfg->mod_name_len,
                               main_cfg->server_port,
                               users,
                               NULL,
                               0,
                               main_cfg->end_hour,
                               main_cfg->end_min,
                               main_cfg->end_day,
                               0,
                               glob_on_connect,
                               ph_parse_rom_message,
                               0,
                               ci_async_write_callback,
                               ph_build_non_fix_rom_pos, switchboard);
        }
    int oat_len = strlen(argv[3]);
    con_interface *oats = create_oats_connection(argv[3], oat_len, gk);
    oats_t *oat = create_oats_struct(oats);
    dcs->oat = oat;
    struct async_parse_args *pa = create_parse_args(
                                                    oats->connection_conf->
                                                    mod_name,
                                                    oats->connection_conf->
                                                    mod_name_len,
                                                    oats->connection_conf->
                                                    fix_port,
                                                    oats->connection_conf->
                                                    fix_ip,
                                                    oats->connection_conf->
                                                    fix_ip_len,
                                                    oats->connection_conf->
                                                    end_hour,
                                                    oats->connection_conf->
                                                    end_min,
                                                    oats->connection_conf->
                                                    end_day,
                                                    oats->connection_conf->
                                                    heart_beat_int,
                                                    oats,
                                                    glob_on_connect,
                                                    oats->
                                                    client_mess_parse,
                                                    oats->session_conf,
                                                    oats->connect,
                                                    -1,
                                                    oats,
                                                    ci_async_write_callback,
                                                    0,
                                                    oats->ci_parse_v,
                                                    oats->ci_obj_callback);
    pa->is_server = 0;
    pa->is_logged_on = 0;
    pa->gk = gk;
    pa->ass = async_server;
    //pa->mc = main_cfg;
    oats->gk = gk;
    oats->on_out = on_out;
    oats->on_fill = on_fill;
    oats->on_cancel_order = cancel_single_order;
    oats->on_cancel_all = cancel_all_orders;
    oats->ass = async_server;
    oats->ap = pa;
    oats->should_buffer = 1;
    pa->ass = async_server;
    set_oats_vals(gk, oats, ci_async_send);
    init_oats_seq(pa);
    async_reconnect(pa);
    set_compliance_con(gk, oat);
    struct dartglob_entry *glob;
    for (glob = globlist; glob != 0; glob = glob->next) {
        con_interface *fix_if = create_empty_interface();
        enqueue(dcs->interfaces, fix_if);
        glob->interface = fix_if;
        glob->interface->ci_obj_callback = return_to_sender;
        glob->entrypoint(fix_if, glob->config_file_name,
                         strlen(glob->config_file_name), gk);
        if (fix_if == 0) {
            goto done;
        }
        
        char destid[10];
        memcpy(destid, fix_if->connection_conf->dest_id, fix_if->connection_conf->dest_id_len);
        destid[fix_if->connection_conf->dest_id_len] = '\0';
        
        /*load_con_schedule(&(glob->interface->ds), 
                          jlc, atoi(destid),
                          oats->session_conf->username,
                          oats->session_conf->un_len);*/
         
        glob->interface->ass = async_server;
        glob->interface->gk = gk;
        glob->interface->on_reject = on_reject;
        glob->interface->on_replaced = on_replaced;
        glob->interface->on_out = on_out;
        glob->interface->on_cancel_all = cancel_all_orders;
        glob->interface->on_fill = on_fill;
        glob->interface->ap = 0;
        glob->interface->recon_wrapper = 0;
        add_destination(gk, (char *) fix_if->connection_conf->dest_id,
                        fix_if->connection_conf->dest_id_len, fix_if);
        add_mapping(fix_if->connection_conf->dest_id,
                    fix_if->connection_conf->dest_id_len,
                    fix_if->session_conf->name,
                    fix_if->session_conf->name_len);
        /* Start the connection for each destination */
            int worked = async_connection(async_server, 
                         glob->interface->connection_conf->mod_name,
                         glob->interface->connection_conf->mod_name_len,
                         glob->interface->connection_conf->fix_port,
                         glob->interface->connection_conf->fix_ip,
                         glob->interface->connection_conf->fix_ip_len,
                         glob->interface->connection_conf->end_hour,
                         glob->interface->connection_conf->end_min,
                         glob->interface->connection_conf->end_day,
                         glob->interface->connection_conf->heart_beat_int,
                         glob->interface,
                         glob_on_connect,
                         glob->interface->client_mess_parse,
                         glob->interface->session_conf,
                         glob->interface->connect,
                         glob->interface,
                         ci_async_write_callback,
                         glob->interface->ci_parse_v,
                         glob->interface->ci_obj_callback);
        
            if(worked == 0) {
                glob->interface->recon_wrapper = add_timer(async_server, 0,
                                0, 30, 0, glob->interface,reconnect);
            }
               
        /*int seconds_to_block = get_seconds_toblock(&(glob->interface->ds));
        if (seconds_to_block >= 0)  //something dramatically wrong if seconds_to_block < 0
        {
            glob->interface->pendingblock = 1;
            glob->interface->block_wrapper = add_timer(async_server, 0,
                        0, seconds_to_block, 0, glob->interface, block);
        }*/
    }
  done:
    return dcs;
}

void* reconnect(void *ci)
{
    con_interface *con = (con_interface *) ci;
    async_server_con_set* gk = con->ass;
    reload_config(con->connection_conf);
    reload_init_args(con->session_conf);
    int worked = async_connection(gk,
                     con->connection_conf->mod_name,
                     con->connection_conf->mod_name_len,
                     con->connection_conf->fix_port,
                     con->connection_conf->fix_ip,
                     con->connection_conf->fix_ip_len,
                     con->connection_conf->end_hour,
                     con->connection_conf->end_min,
                     con->connection_conf->end_day,
                     con->connection_conf->heart_beat_int,
                     con,
                     glob_on_connect,
                     con->client_mess_parse,
                     con->session_conf,
                     con->connect,
                     con,
                     ci_async_write_callback,
                     con->ci_parse_v, con->ci_obj_callback);
    if(worked == 0) {
        //stop installing reconnect timer if session
        //end time has reached and the block flag is on(allowing override!!!)
       // if (should_session_up(con) == 1) 
        {
            if(con->recon_wrapper == 0) {
                con->recon_wrapper = add_timer(con->ass, 0,
                                0, 30, 0, con,reconnect);
            } else {
                add_back_recon_timer(con->ass, con->recon_wrapper);
            }
        }
    }
    return NULL;
}

void* block(void* ci)
{
    con_interface *con = (con_interface *) ci;
    
    if (con->pendingblock)  //blocking
    {
        if (!con->adminblockoverride && con->ap)
        {
            con->is_blocked = 1;
            //con->on_cancel_all(con, NULL);
            if(con->ap) {
                send_oats_update(con->ap, con->connected, con->dest_id,
                         con->dest_len, con->ap->is_server, con);
            }
        }
        
        con->pendingblock = 0;
        con->ds.t_next_end += con->ds.t_period;
        
        int seconds_to_unblock = get_seconds_tounblock(&(con->ds));
        if (seconds_to_unblock >= 0) //something dramatically wrong if seconds_to_unblock < 0
        {
            con->block_wrapper = add_timer(con->ass, 0,
                    0, seconds_to_unblock, 0, con, block);
        }
    }
    else //unblocking
    {
        if (!con->adminblockoverride && con->ap)
        {
            con->is_blocked = 0;
            if(con->ap) {
                send_oats_update(con->ap, con->connected, con->dest_id,
                         con->dest_len, con->ap->is_server, con);
            }
            
            if (!con->connected)
            {
                reconnect(con);
            }
        }
        
        con->pendingblock = 1;
        con->ds.t_next_reset += con->ds.t_period;
        
        int seconds_to_block = get_seconds_toblock(&(con->ds));
        if (seconds_to_block >= 0) //something dramatically wrong if seconds_to_block < 0
        {
            con->is_blocked = 0;
            con->block_wrapper = add_timer(con->ass, 0,
                    0, seconds_to_block, 0, con, block);
        }
    }

    return NULL;
}
