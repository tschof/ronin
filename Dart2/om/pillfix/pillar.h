/**************************************************************************
 * Copyright (c) 2008 by Ronin Capital, LLC
 *
 * All Rights Reserved
 *************************************************************************/

#ifndef _PILLAR_H_
#define _PILLAR_H_

#include "config_loader.h"
typedef struct pillar_parser pparse;

pparse* create_pillar_parser(dart_config *dc);
const char* get_dquote_mne(pparse* b, char* clr_acc, int rlen,
                       int* slen);
const char* get_nyse_flip(pparse* b, char* clr_acc, int clen,
        int* slen);
const char* get_dquote_route(pparse* pp, char* dest, int dlen, int *ret);
const char* get_dquote_clear(pparse* pp, char* dest, int dlen, int *ret);
const char* get_equote_clear(pparse* pp, char* dest, int dlen, int *ret);
const char* get_clearing_id(pparse* pp, char* dest, int dlen, int *ret);
const char* get_default_on_behalf(pparse* pp, int* ret);    
#endif


