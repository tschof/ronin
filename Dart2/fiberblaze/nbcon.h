/***********************************************************************************************************
 * Copyright (c) 2017 by DART Executions LLC
 *
 * All rights reserverd
 *
 * NBCon defines the structures that one uses to control the Dart NYSE binary gateway.
 * The top level DCS structure 
 * *********************************************************************************************************/


#ifndef _NBCON_H__
#define _NBCON_H__

#if defined(__cplusplus)
extern "C" {
#endif
#include <stdint.h>
#include "utp.h"
/*Use this to start and stop the underlying system.*/
typedef struct dart_nb_cntr_structs dnbcntr;

typedef struct dart_nb_parser dnbparse;

typedef struct dart_nb_connection {
    dnbcntr* cntr;
    dnbparse* dnbp;
    void(*on_rej)(utprej* doj);
    void(*on_out)(utpout* doj);
    void(*on_fill)(utpfill* doj);
    void(*on_vfill)(utpvfill* doj);
    void(*on_ack)(utpack* doj);
    void(*on_cnl_ack)(utpcnlack* doj);
    void(*on_connect)(uint8_t is_connected);
} dnbcon;

dnbcon* start_dart(char* password, int plen,
        char* user, int ulen,
        char* config, int cfglen,
        void(*on_rej)(utprej* doj),
        void(*on_out)(utpout* doj),
        void(*on_fill)(utpfill* doj),
        void(*on_vfill)(utpvfill* doj),
        void(*on_ack)(utpack* doj),
        void(*on_cnl_ack)(utpcnlack* doj),
        void(*on_connect)(uint8_t is_connected));

uint8_t send_new_order(dnbcon* dnb, utpno* uno);
uint8_t send_cancel(dnbcon* dnb, utpcnl* ucnl);

#if defined(__cpuluspuls)
}
#endif
#endif
