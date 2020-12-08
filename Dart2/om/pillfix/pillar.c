/**************************************************************************
 * Copyright (c) 2008 by Ronin Capital, LLC
 *
 * All Rights Reserved
 *************************************************************************/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "pillar.h"
#include "service_bureau.h"
#include "dart_allocator.h"


struct pillar_parser {
    sbureau_match* dquote_routes;
    sbureau_match* clearing_ids;
    sbureau_match* clearing_accounts;
    sbureau_match* dquote_mne_map;
    sbureau_match* dquote_clr_map;
    sbureau_match* equote_mne_map;
    char* on_behalf;
    int oblen;
};


pparse* create_pillar_parser(dart_config *dc)
{
    int ret_len = 0;
    pparse* con = 0x0;
    int ret = 0;
    DART_ALLOC(con,8, sizeof(struct pillar_parser), ret) 
    char* directory = get_val_for_tag(dc, "FIX SERVER", 10,
            "idtomnemonic", 9, &ret_len);
    if(ret_len <=0 ) {
        con->clearing_ids = create_service_bureau_matcher("IDtomnemonic.map");
    } else {
        con->clearing_ids = create_service_bureau_matcher(directory);
    }
    char* flipper = get_val_for_tag(dc, "FIX SERVER", 10,
            "GSFlip.map", 9, &ret_len);
    if(ret_len <= 0) {
        flipper = "GSFlip.map";
    }
    con->clearing_accounts = create_service_bureau_matcher(flipper);
    char* dquote_route =
        get_val_for_tag(dc, "FIX SERVER", 10, "DQUOTE ROUTE",
                        12, &ret_len);
    con->dquote_routes = create_service_bureau_matcher(dquote_route);
    char* dquote_mne =
        get_val_for_tag(dc, "FIX SERVER", 10, "DQUOTE MNE",
                        10, &ret_len);
    con->dquote_mne_map = create_service_bureau_matcher(dquote_mne);
    char* equote_mne =
        get_val_for_tag(dc, "FIX SERVER", 10, "EQUOTE MNE",
                        10, &ret_len);
    if(ret_len <=0) {
        equote_mne = "DTR";
    }
    con->equote_mne_map = create_service_bureau_matcher(equote_mne);
    char* dquote_clear =
        get_val_for_tag(dc, "FIX SERVER", 10, "DQUOTE CLEAR",
                        12, &ret_len);
    con->dquote_clr_map = create_service_bureau_matcher(dquote_clear);
    con->on_behalf =
        get_val_for_tag(dc, "FIX SERVER", 10, "OnBehalfOfCompId",
                        16, &con->oblen);
    return con;
}
const char* get_default_on_behalf(pparse* pp, int* ret) 
{
    *ret = pp->oblen;
    return pp->on_behalf;
}  
const char* get_dquote_mne(pparse* b, char* clr_acc, int rlen,
                       int* slen)
{
    return get_mpid_for_clr_acct(b->dquote_mne_map, clr_acc,
            rlen, slen);
}
const char* get_nyse_flip(pparse* b, char* clr_acc, int rlen,
        int* slen)
{
    return get_mpid_for_clr_acct(b->clearing_accounts, clr_acc,
            rlen, slen);
}
const char* get_dquote_route(pparse* pp, char* rkey, int rlen, int *ret)
{
    return get_mpid_for_clr_acct(pp->dquote_routes, rkey,
            rlen, ret);
}
const char* get_dquote_clear(pparse* pp, char* rkey, int rlen, int *ret)
{
    return get_mpid_for_clr_acct(pp->dquote_clr_map, rkey,
            rlen, ret);
}
const char* get_equote_clear(pparse* pp, char* rkey, int rlen, int *ret)
{
    return get_mpid_for_clr_acct(pp->equote_mne_map, rkey,
            rlen, ret);
}
const char* get_clearing_id(pparse* pp, char* rkey, int rlen, int *ret)
{
    return get_mpid_for_clr_acct(pp->clearing_ids, rkey,
            rlen, ret);
}
