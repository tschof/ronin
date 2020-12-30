/**************************************************************************
 * Copyright (c) 2008 by Ronin Capital, LLC
 *
 * All Rights Reserved
 *************************************************************************/
#ifndef _SERVICE_BUREAU_H__
#define _SERVICE_BUREAU_H__

#if defined(__cplusplus)
extern "C" {
#endif

    typedef struct service_bureau_struct sbureau_match;

    sbureau_match* create_service_bureau_matcher(char* filename);

    char* get_mpid_for_clr_acct(sbureau_match* sbm, char* clr_acct, int cl_acct_len, int* mpid_len);


#if defined(__cplusplus)
}
#endif

#endif

