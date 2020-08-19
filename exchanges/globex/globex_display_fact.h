/**************************************************************************
 * Copyright (c) 2008 by Ronin Capital, LLC
 *
 * All Rights Reserved
 *************************************************************************/

#ifndef _GLOBEX_DISP_FACT_H__
#define _GLOBEX_DISP_FACT_H__

#include "hashmap.h"

#if defined(__cplusplus)
extern "C" {
#endif

    hashmap_t load_display_factors(const char *file_name);

    hashmap_t load_clearing_ids(const char* filename);

    hashmap_t load_sender_sub_ids(const char* filename);
#if defined(__cplusplus)
}
#endif
#endif                          //_GLOBEX_H__
