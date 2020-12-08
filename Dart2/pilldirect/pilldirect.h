/***********************************************************************************************************
 * Copyright (c) 2017 by DART Executions LLC
 *
 * All rights reserverd
 *
 * NBCon defines the structures that one uses to control the Dart pillar binary gateway.
 * The top level DCS structure 
 * *********************************************************************************************************/


#ifndef _PILLCON_H__
#define _PILLCON_H__

#if defined(__cplusplus)
extern "C" {
#endif
#include <stdint.h>
#include "pillar.h"
/*Use this to start and stop the underlying system.*/
typedef struct pillar_cntr_structs pillcntr;

typedef struct pillar_parser pillparse;

typedef struct pillar_connection {
    pillcntr* cntr;
    pillparse* dnbp;
    void(*on_rej)(pillcnlreprej* doj);
    void(*on_out)(pillmodcnlack* doj);
    void(*on_fill)(pillexerpt* doj);
    void(*on_ack)(pillordack* doj);
    void(*on_connect)(uint8_t is_connected);
} pillcon;

pillcon* start_pillar(char* password, int plen,
        char* user, int ulen,
        char* config, int cfglen,
        void(*on_rej)(pillcnlreprej* doj),
        void(*on_out)(pillmodcnlack* doj),
        void(*on_fill)(pillexerpt* doj),
        void(*on_ack)(pillordack* doj),
        void(*on_connect)(uint8_t is_connected));
/**
 * These two functions will return 3 possible values
 * 1 - success
 * 0 - short send call flush function.
 * -1 message failed validation.
 *  mf is not checked for null only the length of mf_len is checked.
 *  If mf_len > 0 then it will be attached to the order.
 */
int8_t send_pill_new_order(pillcon* dnb, pillneword* uno, void* mf, int mf_len);
int8_t send_pill_cancel(pillcon* dnb, pillcancel* ucnl);
int8_t flush_pillar(pillparse* cntr);
/**
 * Calls the zf_warm function
 */
void warm_pillar(pillparse* cntr, void* data, int len);
void run_pill_non_block_recv_loop(pillparse* cntr);
void shutdown_pillar(pillparse* cntr);

#if defined(__cplusplus)
}
#endif
#endif
