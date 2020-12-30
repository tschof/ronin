/**************************************************************************
 * Copyright (c) 2008 by Ronin Capital, LLC
 *
 * All Rights Reserved
 *************************************************************************/
#ifndef _UTP_H__
#define _UTP_H__
#if defined(__cplusplus)
extern "C" {
#endif
#pragma pack(push, 1)
    typedef struct utp_new_ord {
        short type;
        short len;
        int seq;
        int qty;
        int max_floor;
        int price;
        char scale;
        char sym[11];
        char exec_inst;
        char side;
        char mtype;
        char tif;
        char cap;
        char route;
        char dot_resv;
        char on_be[5];
        char sender_sub[5];
        char clearing[5];
        char account[10];
        char clordid[17];
        char filler[3];
    }utpno;

    typedef struct utp_cancel {
        short type;
        short len;
        int seq;
        int meordid;
        int orig_ord_qty;
        int cnl_qty;
        int leaves;
        char sym[11];
        char side;
        char on_be[5];
        char sender_sub[5];
        char account[10];
        char clordid[17];
        char orig_clordid[17];
        char filler[2];
    }utpcnl;

    typedef struct utp_ord_ack {
        short type;
        short len;
        int seq;
        int meordid;
        int time;
        char deliver[5];
        char target[5];
        char account[10];
        char clordid[17];
        char filler[3];
    }utpack;

    typedef struct utp_cnl_req_ack {
        short type;
        short len;
        int seq;
        int meordid;
        int time;
        char deliver[5];
        char target[5];
        char account[10];
        char clordid[17];
        char filler[3];
    }utpcnlack;

    typedef struct utp_urout {
        short type;
        short len;
        int seq;
        int meordid;
        int time;
        char info_code;
        char deliver[5];
        char target[5];
        char account[10];
        char orig_clordid[17];
        char filler[2];
    }utpout;

    typedef struct utp_rej {
        short type;
        short len;
        int seq;
        int meordid;
        int time;
        short reason;
        char rej_msg_type;
        char deliver[5];
        char target[5];
        char account[10];
        char clordid[17];
        char orig_clordid[17];
        char text[40];
        char filler[3];
    }utprej;

    typedef struct utp_bust {
        short type;
        short len;
        int seq;
        int meordid;
        int time;
        int qty;
        int price;
        char pscale;
        char b_or_c;
        char deliver[5];
        char target[5];
        char contrab[5];
        char contrat[5];
        char execid[10];
        char exec_ref_id[10];
        char account[10];
        char clordid[17];
        char filler[3];
    }utpbust;

    typedef struct utp_fill {
        short type;
        short len;
        int seq;
        int meordid;
        int time;
        int leaves;
        int last;
        int price;
        char pscale;
        char side;
        char bill_code;
        char last_mkt;
        char deliver[5];
        char target[5];
        char exec_broker[5];
        char contrab[5];
        char contrat[5];
        char exec_away_mkt_id[6];
        char billing_rate[6];
        char execid[10];
        char account[10];
        char db_exec_id[10];
        char clordid[17];
    }utpfill;

    typedef struct verbose_fill {
        short type;
        short len;
        int seq;
        int meordid;
        int time;
        int contr_trade_time;
        int last;
        int lprice;
        int qty;
        int leaves;
        int cum;
        int avg;
        char lppscale;
        char avgppscale;
        char exec_type;
        char exec_trans_type;
        char ord_type;
        char tiff;
        char side;
        char status;
        char last_mkt;
        char billing;
        char capacity;
        char auto_ex;
        char poss_resend;
        char deliver[5];
        char target[5];
        char clearing[5];
        char exec_broker[5];
        char contrab[5];
        char contrat[5];
        char exec_away_mktid[6];
        char billing_rate[6];
        char exec_id[10];
        char db_exec_id[10];
        char account[10];
        char sym[11];
        char clordid[17];
        char orig_clorid[17];
        char filler[2];
    }utpvfill;
#pragma pack(pop)
#if defined(__cplusplus)
}
#endif
#endif

