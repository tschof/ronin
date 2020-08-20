#ifndef __MARKET_DATA_SOURCE_HPP__
#define __MARKET_DATA_SOURCE_HPP__

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
  class Market_Data_Source
  {
  public:
    virtual int subscribe_symbol (const char*) = 0;
    virtual int unsubscribe_symbol (const char*) = 0;
    virtual Price_Block *get_price_block (const char*) const = 0;
    virtual Price_Block *get_price_block (const std::string&) const = 0;
  };
}

#endif
