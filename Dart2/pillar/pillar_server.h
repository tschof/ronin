/**************************************************************************
 * Copyright (c) 2008 by Ronin Capital, LLC
 *
 * All Rights Reserved
 *************************************************************************/
#ifndef _PILLAR_SERVER_H__
#define _PILLAR_SERVER_H__
#if defined(__cplusplus)
extern "C" {
#endif

int pillar_to_rom_price(int64_t price, databuf_t* val);
int pillar_to_rom_time(uint64_t eur_time, databuf_t* buff);

#if defined(__cplusplus)
}
#endif
#endif
