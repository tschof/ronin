/**************************************************************************
 * Copyright (c) 2008 by Ronin Capital, LLC
 *
 * All Rights Reserved
 *************************************************************************/
#ifndef _BOE_MASTER_H__
#define _BOE_MASTER_H__

#include <pthread.h>

#include "service_bureau.h"
#include "gatekeeper.h"
#include "polling_server.h"
#include "connection_interface.h"
#include "call_back_wrapper.h"
#include "suffix_map.h"
#include "boe_seq.h"
#include "log_mnger.h"



#pragma pack(push, 1)
struct boe_logon {
    unsigned short som;
    unsigned short len;
    unsigned char type;
    unsigned char mu;
    unsigned int seq;
    char sender_sub_id[4];
    char username[4];
    char password[10];
    unsigned char num_params;
};
#pragma pack(pop)
typedef struct boe_master boem;
struct boe_master {
    otl* o;
    login_info* logon;
    con_int* ci;
    rex_con* rc;
    gatekeeper* gk;
    bsm* bseq;
    token_gen* tg;
    sbureau_match* sbm;
    sbureau_match* prod_codes;
    sbureau_match* rom_to_cfe_simp;
    suffixm* suffix_map;
    suffixm* clr_accs;
    databuf_t* mess_to_send;
    pthread_spinlock_t send_mutex;
    call_back_wrapper* recon_wrap;
    call_back_wrapper* hb_timer;
    int is_logged_on;
    time_t last_sent;
    char* username;
    uint16_t ulen;
    char* password;
    uint16_t plen;
    char* sendsub;
    uint16_t sslen;
    uint64_t seq;
    void (*parse_seq_packet)(boem *pc,
                      char* message,
                      size_t mess_len,
                      char mtype, 
                      short mlen, long bu);
};

void send_boe_logout(boem* ob);
void boe_direct_send(boem* up, void* data, int len);
void parse_boe(int *cut_con, rex_con *pc);
#endif //_BOE_MASTER_H__
