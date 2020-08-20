// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the SRLABS_MARKET_DATA_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// SRLABS_MARKET_DATA_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifdef SRLABS_MARKET_DATA_EXPORTS
#define SRLABS_MARKET_DATA_API __declspec(dllexport)
#else
#define SRLABS_MARKET_DATA_API __declspec(dllimport)
#endif

#include <Market_Data/DART_Market_Data_Source_Interface.h>

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * SRLabs_Market_Data DLL
 *
 * This DLL implements the uniform DLL interface to be used across market data
 * providers.  The goal is to insulate the DLL user from the details of
 * different market data providers as much as possible.  The interface is
 * entirely in C, with no C++ used.  This insulates the DLL user from the
 * version of compiler used to generate the DLL interface.
 */

SRLABS_MARKET_DATA_API  int DART_MD_init(const char *inifile,const char *section);
SRLABS_MARKET_DATA_API  int DART_MD_subscribe(const char *symbol);
SRLABS_MARKET_DATA_API  int DART_MD_unsubscribe(const char *symbol);
SRLABS_MARKET_DATA_API  int DART_MD_init_price_block(Price_Block *pblock);
SRLABS_MARKET_DATA_API  int DART_MD_get_price_block(const char *symbol,Price_Block *pblock);
SRLABS_MARKET_DATA_API void DART_MD_shutdown();

#ifdef __cplusplus
}
#endif
