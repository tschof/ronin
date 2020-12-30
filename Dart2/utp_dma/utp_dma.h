/**************************************************************************
 * Copyright (c) 2008 by Ronin Capital, LLC
 *
 * All Rights Reserved
 *************************************************************************/
#ifndef _UTP_DMA_H__
#define _UTP_DMA_H__
#if defined(__cplusplus)
extern "C" {
#endif
#include "polling_server.h"

int get_utp_field_len(char *start, int max_len);
void parse_con_utpdirect(int *cut_con, rex_con *rc);

#if defined(__cplusplus)
}
#endif
#endif
