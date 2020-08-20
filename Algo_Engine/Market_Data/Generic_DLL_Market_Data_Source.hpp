#ifndef __GENERIC_DLL_MARKET_DATA_SOURCE_HPP__
#define __GENERIC_DLL_MARKET_DATA_SOURCE_HPP__

#include <ace/Singleton.h>
#include <string>
#include "Market_Data_Source.hpp"
#include "DART_Market_Data_Source_Interface.h"

namespace dart
{
  /**
   * @class Market_Data_Source
   *
   * @brief Interface for a generic market data source, allows us to eventually
   * switch out market data implementations, if needed.
   *
   * Even though the Wombat API provides an interface to various forms of Wombat
   * data, it is still Wombat.  If we need to move to a different market data
   * provider, the new provider will simply implement this interface and the rest
   * of the application will be none the wiser.
   */
  class Generic_DLL_Market_Data_Source
    : public Market_Data_Source
  {
  public:
    Generic_DLL_Market_Data_Source();
    virtual int open(const char *dll,
                     const char *configfile,
                     const char *configsection);
    virtual int subscribe_symbol (const char*);
    virtual int unsubscribe_symbol (const char*);
    virtual Price_Block *get_price_block (const char*) const;
    virtual Price_Block *get_price_block (const std::string&) const;
    virtual ~Generic_DLL_Market_Data_Source();
  private:
    HMODULE                        dll_handle_;
    PDART_MD_INIT_FUNC             init_func_;
    PDART_MD_SUBSCRIBE_FUNC        subscribe_func_;
    PDART_MD_UNSUBSCRIBE_FUNC      unsubscribe_func_;
    PDART_MD_INIT_PRICE_BLOCK_FUNC init_price_block_func_;
    PDART_MD_GET_PRICE_BLOCK_FUNC  get_price_block_func_;
    PDART_MD_SHUTDOWN              shutdown_func_;
  };

  typedef ACE_Singleton <Generic_DLL_Market_Data_Source,
                         ACE_SYNCH_MUTEX> GENERIC_MD_SOURCE;
}

#endif
