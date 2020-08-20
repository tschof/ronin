#ifndef __MARKET_DATA_CLIENT_HPP__
#define __MARKET_DATA_CLIENT_HPP__

namespace dart
{
  /**
   * @class Market_Data_Client
   * @brief Pure virtual interface for dart clients of market data.
   *
   * Provides an interface for the rest of the dart libraries to receive market
   * data without being tied directly to the current implementation.  Currently
   * only provides error condition information when data feeds are not available
   * but can be expanded in the future to deliver market data updates.
   */

  class Market_Data_Client
  {
    virtual void on_error (const char * /*symbol*/) = 0;
  };
}

#endif