/**************************************************************************
 * Copyright (c) 2008 by Ronin Capital, LLC
 *
 * All Rights Reserved
 *************************************************************************/

#ifndef _OUCH_PARSER_H__
#define _OUCH_PARSER_H__

#include "databuf.h"
#include "connection_interface.h"
#include "dart_order_obj.h"
#include "otto.h"
#include "order_token_generator.h"
#include "risk_cntr_structs.h"


ottono* create_otto_new_order_message(rcntr *rph,
                             ottop * ob, con_int* ci,token_gen* tg);
ottocnl* create_otto_cancel_message(rcntr* rph,
                          ottop * ob, con_int* ci,token_gen* tg);
ottorep* create_otto_replace_message(rcntr* rph,
                           ottop * ob, con_int* ci,token_gen* tg);
ottono* create_otto_new_cmplx(rcntr* rph, 
                            ottop* ob, con_int* ci, token_gen* tg,
                            uint32_t instrumentid, char* mpid, int mpidlen);
int64_t rom_to_otto_price(char *price, size_t len);
int otto_to_rom_price(int64_t price, databuf_t* val);
void init_today();
void set_date_str(char *output, long tt);

void update_order_ack(rcntr* obj, ottooa* oak, databuf_t* db);
void parse_replaced(rcntr* obj, ottorepl* orpl, con_int* ci);
void parse_ur_out(rcntr* obj,ottocnled* ocnl);
rcntr* parse_filled(rcntr* obj,ottoexe* oexe, databuf_t* db, ottop *otp);
void update_short_order_ack(rcntr* obj,
        ottooashorty* oak);

int parse_rej(rcntr* obj, ottorej* rej);
#endif
