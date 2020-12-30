static boeparse* create_boe_con(boecon* dnbc, boecntr* cntr,
        char* config, int cfglen, char* user, int ulen, con_int* ci)
{
    int utp_len = 0;
    char* utpnum = get_config_val(cntr->mc, "SERVER", 6,
            "Dest ID", 7, &utp_len);
    ci->ex = create_config_from_db(cntr->jp, config, cfglen, utpnum);
    ci->ex->last_logon = cntr->mc->last_logon;
    ci->mod_name = calloc(cfglen + 1, 1);
    ci->mod_name_len = 0;//;
    memcpy(ci->mod_name, config, cfglen);
    ci->mod_name_len = cfglen;
    ci->ex->dest_id = calloc(utp_len + 1, 1);
    memcpy(ci->ex->dest_id, utpnum, utp_len);
    ci->ex->dest_id_len = utp_len;
    ci->o = get_logger(cntr->ass);
    ci->gk = cntr->gk;
    ci->p = cntr->ass;
    ci->update_oats = nb_log_to_oats_dest;
    ci->can_send_orders = 0;
    ci->cl_ord_id_map = get_initial_order_map(cntr->gk, ci->mod_name,
            ci->mod_name_len);
    ci->sbm = 0;
    ci->tg = 0;
    ci->ex->username = calloc(1, ulen + 1);
    memcpy(ci->ex->username, user, ulen);
    ci->ex->ulen = ulen;
    ci->ex->dc = cntr->mc->dc;

    login_info* li = nb_build_logon_info_from_config(ci->ex, user, ulen);
    llparse* dnbp = create_boear_parser(ci->o, ci, li, dnbc);
    int ret_len = 0;
    char* tmpid = get_config_val(cntr->mc, "SERVER", 6,
            "MPID", 4, &ret_len);
    if(ret_len == 0) {
        dnbp->mpid = "DART";
    } else {
        dnbp->mpid = calloc(1,8);
        memcpy(dnbp->mpid, tmpid, 4);
    }
    dnbp->mic = get_config_val(cntr->mc, "SERVER", 6,
            "MIC", 3, &ret_len);
    dnbp->miclen = ret_len;

    dnbp->user = cntr->mc->username;
    dnbp->ulen = cntr->mc->ulen;
    dnbp->password = cntr->mc->password;
    dnbp->plen = cntr->mc->plen;
    insert_ords_in_tree(ci, dnbp);
    dnbp->trader_lim = get_equity_limits_for_trader(cntr->jp,
            user, ulen);
    ci->ph = dnbp->symbols;
    dnbp->group_lim = get_equity_limits_for_clraccnt(cntr->jp, user, ulen);
    if(dnbp->group_lim == 0x0) {
        dnbp->group_lim = calloc(1,sizeof(struct jp_limits));
        dnbp->group_lim->bp = dnbp->trader_lim->bp;
        dnbp->group_lim->max_doll = dnbp->trader_lim->max_doll;
        dnbp->group_lim->max_ord = dnbp->trader_lim->max_ord;
        dnbp->group_lim->max_shares_ord= dnbp->trader_lim->max_shares_ord;
        dnbp->group_lim->max_share= dnbp->trader_lim->max_share;
        dnbp->group_lim->cur_balance_buy= dnbp->trader_lim->cur_balance_buy;
        dnbp->group_lim->cur_balance_sell= dnbp->trader_lim->cur_balance_sell;
        dnbp->group_lim->open_ords= dnbp->trader_lim->open_ords;
        dnbp->group_lim->open_shares= dnbp->trader_lim->open_shares;
        dnbp->group_lim->mrate= dnbp->trader_lim->mrate;
        dnbp->group_lim->nmes= dnbp->trader_lim->nmes;
        dnbp->group_lim->toff= dnbp->trader_lim->toff;
    }
    dnbp->trader =  jp_get_trader(cntr->jp, user, ulen);
    ci->logon = li;
    ci->cparser = dnbp;
    dnbc->dnbp = dnbp;
    add_destination(cntr->gk, ci->ex->dest_id, ci->ex->dest_id_len,
            ci);
    rex_con* rc = create_rex_con(cntr->ass, ci->ex, dnbp, dnbp,li,
            pl_write_callback,
            boe_client_connect,
            plparse_boe, NULL);
    rc->parser = dnbp;
    rc->connect = plsend_boear_logon;
    dnbp->rc = rc;
    ci->rc = rc;
    return dnbp;
}
void shutdown_boe(boeparse* cntr)
{
}

boecon* start_boe(char* password, int plen,
        char* user, int ulen,
        char* config, int cfglen,
        void(*on_rej)(boecnlreprej* doj),
        void(*on_out)(boemodcnlack* doj),
        void(*on_fill)(boeexerpt* doj),
        void(*on_ack)(boeordack* doj),
        void(*on_connect)(uint8_t is_connected))
{
    int mod_name_len = 0;
    boecon* dbc = malloc(sizeof(boecon));
    dbc->cntr = malloc(sizeof(boecntr));
    dbc->cntr->mc = init_config(config, cfglen);
    dbc->cntr->mc->mod_name = get_mod_name_for_user(dbc->cntr->mc,
            user, &mod_name_len);

    if(mod_name_len <= 0) {
        destroy_ex_config(dbc->cntr->mc);
        free(dbc->cntr);
        free(dbc);
        return NULL;
    }
    if(access("/var/log/ptpd2.status", F_OK) == 0) {
        dbc->on_rej = on_rej;
        dbc->on_out = on_out;
        dbc->on_fill = on_fill;
        dbc->on_ack = on_ack;
        dbc->on_connect = on_connect;
        time_t now;
        time(&now);
        dbc->cntr->ass = start_lib_base_services(dbc->cntr->mc);
        dbc->cntr->jp = create_jp_limits_checker(dbc->cntr->mc,
                get_logger(dbc->cntr->ass),0x0, 0);
        sbrouter* dd = create_direct_router(get_logger(dbc->cntr->ass),
                dbc->cntr->mc->mod_name,
                dbc->cntr->mc->mod_name_len);
        dbc->cntr->mc->create_new = ll_before_reset_time(dbc->cntr->mc->reset_day,
                dbc->cntr->mc->reset_hour,
                dbc->cntr->mc->reset_min,
                dbc->cntr->mc->last_logon, now);

        dbc->cntr->gk = create_dd_gatekeeper(dd, get_logger(dbc->cntr->ass),dbc->cntr->ass,
                dbc->cntr->mc->mod_name, dbc->cntr->mc->mod_name_len,
                dbc->cntr->jp, dbc->cntr->mc->create_new);
        dbc->cntr->cm = create_gen_life_master(dbc->cntr->ass, dbc->cntr->mc);
        load_orders(dbc->cntr->jp, dbc->cntr->gk, dbc->cntr->mc->destinations);
        start_dart_mail_service(dbc->cntr->mc->mail_server, dbc->cntr->mc->mail_server_len,
                dbc->cntr->mc->biz_group, dbc->cntr->mc->b_len,
                dbc->cntr->mc->tech_group, dbc->cntr->mc->t_len,
                dbc->cntr->mc->end_hour, dbc->cntr->mc->end_min,
                dbc->cntr->mc->end_day);
        dbc->cntr->oats = create_oats(dbc->cntr->mc->mod_name,
                dbc->cntr->mc->mod_name_len, dbc->cntr);
        con_int* ci = create_empty_interface();
        dbc->dnbp = create_boe_con(dbc, dbc->cntr,dbc->cntr->mc->mod_name,
                dbc->cntr->mc->mod_name_len, user, ulen, ci);
        send_oats_update(dbc->cntr->oats, 1, user, ulen, 1, dbc->dnbp->boe);
        rex_connect(dbc->cntr->ass, dbc->dnbp->rc);
    } else {
        printf("PTP not detected!!! %d \n\n", errno);
        destroy_ex_config(dbc->cntr->mc);
        free(dbc->cntr);
        free(dbc);
        return NULL;
    }
    return dbc;
}

