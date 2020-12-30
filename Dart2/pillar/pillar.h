/**************************************************************************
 * Copyright (c) 2008 by Ronin Capital, LLC
 *
 * All Rights Reserved
 *************************************************************************/
#ifndef _PILLAR_H__
#define _PILLAR_H__
#if defined(__cplusplus)
extern "C" {
#endif

#define PILL_CNL_REP_REJ 0x0263
#define PILL_BUST 0x0292
#define PILL_EXE 0x0290
#define PILL_PRI_UPDATE 0x0262
#define PILL_MOD_OUT_ACK 0x0271
#define PILL_ORD_ACK 0x0260
#define PILL_MASS_CNL_REQ 0x0281
#define PILL_MOD_REQ 0x0270

#pragma pack(push, 1)

typedef struct pillar_msg_header {
    uint16_t type;
    uint16_t length;
} pmh;

typedef struct pillar_stream_id {
    uint32_t sess;
    uint32_t value;
} psid;

typedef struct pillar_seq_msg_id {
    uint64_t id;
    uint64_t seq;
} psmid;

typedef struct pillar_stream_avail {
    pmh header;
    uint64_t stream_id;
    uint64_t next_seq;
    uint8_t access;
}psavail;

typedef struct pillar_heart_beat {
    pmh header;
}phb;

typedef struct pillar_open {
    pmh header;
    uint64_t stream_id;
    uint64_t start_seq;
    uint64_t end_seq;
    uint8_t access;
    uint8_t mode;
} pillopen;

typedef struct pillar_open_reponse {
    pmh header;
    uint64_t stream_id;
    uint8_t status;
    uint8_t access;
} popen_resp;

typedef struct pillar_close {
    pmh header;
    uint64_t stream_id;
} pillclose;

typedef struct pillar_close_resp {
    pmh header;
    uint64_t stream_id;
    uint8_t status;
} pclose_resp;

typedef struct pillar_login {
    pmh header;
    char username[16];
    char password[32];
    char mic[4];
    char version[20];
}pillog;

typedef struct pillar_login_resp {
    pmh header;
    char username[16];
    uint8_t status;
} plr;

typedef struct pillar_seq_msg {
    pmh header;
    psmid seq_msg_id;
    uint32_t reserve;
    uint64_t timestamp;
} psm;

typedef struct pillar_reference_data_struct {
    pmh header;
    uint64_t timestamp;
    uint32_t symbol;
    char nyse_sym[24];
    char listedmic[4];
    uint8_t round_lot_size;
    uint64_t max_ord_price;
    uint16_t mpv_class_id;
    uint8_t test_sym_ind;
}pill_ref_data;
typedef struct pillar_mpid_configuration {
    pmh header;
    uint64_t timestamp;
    uint8_t status;
    char mpid[4];
    char username[16];
} mpid_config;

typedef struct pillar_session_ack {
    pmh header;
    uint64_t timestamp;
    uint8_t user_session_type;
    uint8_t user_session_status;
    char username[16];
    char mic[4];
    uint8_t cancel_on_disconnect;
    uint8_t throttle_pref;
    uint16_t throttle_window;
    uint16_t throttle_thresh;
    uint8_t symbol_eligiblity;
    uint32_t max_order_qty;
    uint8_t self_trade_prev;
    uint8_t order_priority_update;
    uint8_t ack_status;
} pillsessack;

typedef struct pillar_session_config_req {
    pmh header;
    char username[16];
    uint8_t cancel_on_disconnect;
    uint8_t throttle_pref;
    uint8_t self_trade_prev;
    uint8_t order_priority_update;
} pillsessconfig;

typedef struct pillar_new_order_request {
    pmh header;
    uint32_t symbol;
    char mpid[4];
    uint32_t mmid;
    char mpsubid;
    uint64_t clordid;
    uint64_t orig_ord_key;
    uint64_t bitfield_instr;
    uint64_t price;
    uint32_t qty;
    uint32_t min_qty;
    char user_data[8];
    uint32_t optional;
} pillneword;

typedef struct pillar_cancel_request {
    pmh header;
    uint32_t symbol;
    char mpid[4];
    uint64_t clordid;
    uint64_t orig_ord_key;
} pillcancel;

typedef struct pillar_modify_request {
    pmh header;
    uint32_t symbol;
    uint64_t clordid;
    uint64_t orig_ord_key;
    uint32_t qty;
    uint8_t side;
    uint8_t locate;
} pillmodify;

typedef struct pillar_mass_cancel_request {
    pmh header;
    uint32_t symbol;
    char mpid[4];
    uint32_t mmid;
    uint64_t clordid;
    char deliver_to_comp[5];
    uint8_t bulk_cancel_type;
    uint8_t side;
} pillmasscancel;

typedef struct pillar_order_ack {
    pmh header;
    uint64_t timestamp;
    uint32_t symbol;
    char mpid[4];
    uint32_t mmid;
    char mpsubid;
    uint64_t clordid;
    uint64_t orig_clordid;
    uint64_t bitfield_instr;
    uint64_t price;
    uint32_t qty;
    uint32_t min_qty;
    uint64_t order_id;
    uint32_t leaves;
    uint64_t working_price;
    uint8_t wadisplay;
    char pre_liq_ind[4];
    uint16_t reason;
    uint8_t acktype;
    uint8_t bitfieldflow;
    char user_data[8];
} pillordack;

typedef struct pillar_modify_cancel_ack {
    pmh header;
    uint64_t timestamp;
    uint32_t symbol;
    char mpid[4];
    uint64_t order_id;
    uint64_t refclordid;
    uint64_t orig_clordid;
    uint64_t price;
    uint32_t qty;
    uint32_t leaves;
    uint8_t side;
    uint8_t locate;
    uint16_t reason;
    uint8_t acktype;
    uint8_t bit_indicator;
    char user_data[8];
} pillmodcnlack;

typedef struct pillar_priority_update_ack {
    pmh header;
    uint64_t timestamp;
    uint32_t symbol;
    char mpid[4];
    uint64_t order_id;
    uint64_t clordid;
    uint64_t workingprice;
    uint32_t qty;
    uint8_t wadisplay;
    char preliqind[4];
    uint8_t acktype;
    char user_data[8];
} pillpriorack;

typedef struct pillar_execution_report {
    pmh header;
    uint64_t timestamp;
    uint32_t symbol;
    char mpid[4];
    uint64_t order_id;
    uint64_t  clordid;
    uint64_t dealid;
    uint64_t last_price;
    uint32_t leaves;
    uint32_t cum;
    uint32_t lastqty;
    char liq[4];
    char displiq[4];
    uint8_t locate;
    uint8_t participanttype;
    uint16_t reason;
    char user_data[8];
} pillexerpt;


typedef struct pillar_bust_correct {
    pmh header;
    uint64_t timestamp;
    uint32_t symbol;
    char mpid[4];
    uint64_t order_id;
    uint64_t clordid;
    psmid refseqmsgid;
    uint64_t dealid;
    uint64_t last_price;
    uint32_t lastqty;
    uint16_t reason;
    char user_data[8];
} pillbust;

typedef struct pillar_cnl_rep_mod_rej {
    pmh header;
    uint64_t timestamp;
    uint32_t symbol;
    char mpid[4];
    uint64_t clordid;
    uint16_t reason;
    uint8_t rejtype;
    char user_data[8];
    char res[4];
} pillcnlreprej;

typedef struct pillar_equity_cust_addon {
    pmh header;
    char deliver[5];
    uint32_t maxfloor;
    uint32_t minpegqty;
    uint64_t discpricerng;
    uint64_t offsetprice;
    uint64_t effectivetime;
} pillequityadd;

typedef struct pillar_floor_broker_long_addon {
    pmh header;
    char parentmpid[4];
    uint64_t parentclordid;
    char badgeid[4];
    char billto[5];
    uint32_t maxfloor;
    uint32_t minpegqty;
    uint64_t discpricerng;
    uint64_t offsetprice;
    uint8_t display;
} pillflobrolong;

typedef struct pillar_floor_broker_short_addon {
    pmh header;
    char parentmpid[4];
    uint64_t parentclordid;
    char badgeid[4];
    char billto[5];
} pillflobroshort;

#pragma pack(pop)


#if defined(__cplusplus)
}
#endif
#endif
