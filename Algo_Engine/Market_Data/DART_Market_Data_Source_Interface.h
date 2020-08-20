#ifndef __DART_MARKET_DATA_SOURCE_INTERFACE_H__
#define __DART_MARKET_DATA_SOURCE_INTERFACE_H__

#include "DART_Price_Block.h"

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * Function pointers for use when dynamically
 * loading DART market data wrapper DLLs.
 */
typedef  int (*PDART_MD_INIT_FUNC)(const char *inifile,const char *section);
typedef  int (*PDART_MD_SUBSCRIBE_FUNC)(const char *symbol);
typedef  int (*PDART_MD_UNSUBSCRIBE_FUNC)(const char *symbol);
typedef  int (*PDART_MD_INIT_PRICE_BLOCK_FUNC)(Price_Block *pblock);
typedef  int (*PDART_MD_GET_PRICE_BLOCK_FUNC)(const char *symbol,Price_Block *pblock);
typedef void (*PDART_MD_SHUTDOWN)();

#ifdef __cplusplus
}
#endif

#endif
