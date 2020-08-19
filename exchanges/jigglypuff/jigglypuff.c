/**************************************************************************
 * Copyright (c) 2008 by Ronin Capital, LLC
 *
 * All Rights Reserved
 *************************************************************************/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/time.h>


#include "generic_db.h"
#include "ex_config_helper.h"
#include "hashmap.h"
#include "message_queue.h"
#include "common_structs.h"

struct jp_limits_checker {
    char* db_user_name;
    int db_user_name_len;
    char* db_password;
    int db_password_len;
    char* db_server;
    int db_server_len;
    hashmap_t traders;
    queue_t allowed_logons;
};

typedef struct jp_limits {
    double bp;
    double max_doll;
    long max_ord;
    long max_shares_ord;
    long max_share;
    double cur_balance;
    long open_ords;
    long open_shares;
 } jp_limits; 

typedef struct jp_trader_info {
    char* name;
    int name_len;
    char* password;
    int pass_len;
    jp_limits* eq_limits;
    jp_limits* opt_limits;
    jp_limits* fut_limits;
    jp_limits* fx_limits;
    int blocked;
    int exempt;
};


void jp_block_trader(jp_limits_checker* jpc, char* trader, int len, int blocked)
{
    jp_trader_info* jpti = 0;
    int found = get_map_entry(jpc->traders, trader, len, &jpti);
    if(found) {
        jpti->blocked = blocked;
    } 
}

void jp_exempt_trader(jp_limits_checker* jpc, char* trader, int len, int exempt)
{
    jp_trader_info* jpti = 0;
    int found = get_map_entry(jpc->traders, trader, len, &jpti);
    if(found) {
        jpti->exempt = exempt;
    }
}

jp_limits_checker* create_jp_limits_checker(ex_config ex)
{
    jp_limits_checker* jplc = malloc(sizeof *jplc);
    jplc->traders = create_hashmap(256);
    jplc->db_user_name = calloc(ex->db_user_name_len, 1);
    memcpy(jplc->db_user_name, ex->db_user_name, ex->db_user_name_len);
    jplc->dp_user_name_len = ex->db_user_name_len;
    jplc->db_password = calloc(ex->db_password_len, 1);
    memcpy(jplc->db_password, ex->db_password, ex->db_password_len);
    jplc->dp_password_len = ex->db_password_len;
    jplc->db_server = calloc(ex->db_server_len, 1);
    memcpy(jplc->db_server, ex->db_server, ex->db_server_len);
    jplc->dp_server_len = ex->db_server_len;
    load_trader_profiles(jplc);
}

int load_trader_profiles(jp_limits_checker* jc)
{
    queue_t q = create_sized_queue(256);
    jp_trader_info* jpti = 0;
    int res = 0;
    int len = 0;
    gendb_get_results(ex->db_user_name,jc->db_user_name_len, jc->db_password,
                      jc->db_password_len, jc->db_server, jc->db_server_len,
                      *q,"rom.dbo.ROMGetROMusersNew");
    queue_t row = NULL;
    gendb_v* val = NULL;
    element_t erow, ecol;
    while (!is_empty(q)) {
      int cnt = 0;
      erow = pop_front(q);
      row = (queue_t)(erow->d);
      while (!is_empty(row)) {
        ecol = pop_front(row);
        val = (gendb_v*)(ecol->d);
        switch(cnt) {
        case 0:
          len = strlen((val->g_un).un_array); 
          res = get_map_entry(jc->traders, ((val->g_un).un_array), len, &jpti);
          if(res <= 0) {
              jpti = calloc(1, sizeof *jpti);
              jpti->eq_limits = calloc(1, sizeof(struct jp_limits));
              jpti->opt_limits = calloc(1, sizeof(struct jp_limits));
              jpti->fut_limits = calloc(1, sizeof(struct jp_limits));
              jpti->fx_limits = calloc(1, sizeof(struct jp_limits));
              jpti->name = calloc(len + 1, 1);
              memcpy(jpti->name, ((val->g_un).un_array), len);
              jpti->name_len = len;
              no_copy_insert(jc->traders, jpti->name, jpti->name_len, jpti);
          }
          break;
        case 1:
          if(res <= 0) {
              jpti->password = calloc(len + 1, 1);
              memcpy(jpti->password, ((val->g_un).un_array), len);
              jpti->pass_len = len;
              struct init_args* in = calloc(1, sizeof *in);
              in->name = calloc(jpti->name_len + 1, 1);
              memcpy(in->name, jpti->name, jpti->name_len);
              in->name_len = jpti->name_len;
              
              in->username = calloc(jpti->name_len + 1, 1);
              memcpy(in->name, jpti->name, jpti->name_len);
              in->un_len = jpti->name_len;

              in->password = calloc(jpti->pass_len + 1, 1);
              memcpy(in->password, jpti->password, jpti->pass_len);
              in->p_len = jpti->pass_len;
              enqueue(allowed_logons, in);
          }
          break;
        case 13:
          jpti->eq_limits->bp = (pVal->g_un).un_double;
          break;
        case 14:
          jpti->opt_limits->bp = (pVal->g_un).un_double;
          break;
        case 15:
          jpti->fut_limits->bp = (pVal->g_un).un_double;
          break;
        case 16:
          jpti->fx_limits->bp = (pVal->g_un).un_double;
          break;
        case 17:
          jpti->eq_limits->max_shares_ord = (pVal->g_un).un_int;
          break;
        case 18:
          jpti->opt_limits->max_shares_ord = (pVal->g_un).un_int;
          break;
        case 19:
          jpti->fut_limits->max_shares_ord = (pVal->g_un).un_int;
          break;
        case 20:
          jpti->fx_limits->max_shares_ord = (pVal->g_un).un_int;
          break;
        case 21:
          jpti->eq_limits->max_doll = (pVal->g_un).un_double;
          break;
        case 22:
          jpti->opt_limits->max_doll = (pVal->g_un).un_double;
          break;
        case 23:
          jpti->fut_limits->max_doll = (pVal->g_un).un_double;
          break;
        case 24:
          jpti->fx_limits->max_doll = (pVal->g_un).un_double;
          break;
        case 25:
          jpti->eq_limits->max_ord = (pVal->g_un).un_int;
          break;
        case 26:
          jpti->opt_limits->max_ord = (pVal->g_un).un_int;
          break;
        case 27:
          jpti->fut_limits->max_ord = (pVal->g_un).un_int;
          break;
        case 28:
          jpti->fx_limits->max_ord = (pVal->g_un).un_int;
          break;
        case 29:
          jpti->eq_limits->max_shares = (pVal->g_un).un_int;
          break;
        case 30:
          jpti->opt_limits->max_shares = (pVal->g_un).un_int;
          break;
        case 31:
          jpti->fut_limits->max_shares = (pVal->g_un).un_int;
          break;
        case 32:
          jpti->fx_limits->max_shares = (pVal->g_un).un_int;
          break;
        default:
          break;
        }
        ++cnt;
        free(ecol);
        free(val);
      }
      free(erow);
      destroy_queue(row);
    }
    destroy_queue(q);
    return 0;
}
