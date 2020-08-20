#ifndef __WOMBAT_MARKET_DATA_SOURCE_HPP__
#define __WOMBAT_MARKET_DATA_SOURCE_HPP__

#include <mama/types.h>
#include <ace/Singleton.h>
#include <ace/Mutex.h>
#include <set>
#include "Market_Data_Source.hpp"

#pragma warning (push)
#pragma warning (disable:4100)
#pragma warning (disable:4512)
#include <mama/mamacpp.h>
using namespace Wombat;
#pragma warning (pop)

#include <hash_map>

namespace dart
{
  class Data_Callback;

  /**
   * @class Market_Data_Conduit
   *
   * @brief A Singleton object of this class handles all market data
   * subscriptions for the process.
   *
   * This class will be used as a Singleton as it creates a Wombat market data
   * connectiona and all of the associated setup and teardown that goes with
   * that.  It might be important to keep this a Singleton if this setup calls
   * functions that can only be called once (not sure about this, would have to
   * test more extensively to find out.
   */
  class Wombat_Market_Data_Conduit
    : public Market_Data_Source,
      public Wombat::MamaStartCallback,
      public Wombat::MamaSubscriptionCallback
  {
  public:
    Wombat_Market_Data_Conduit ();

    // Market_Data_Source overrides
    virtual int subscribe_symbol (const char*);
    virtual int unsubscribe_symbol (const char*);
    virtual Price_Block *get_price_block (const char *symbol) const;
    virtual Price_Block *get_price_block (const std::string&) const;

    // Wombat::MamaStartCallback override
    virtual void onStartComplete (MamaStatus status);

    // MamaSubscriptionCallback virtual method overrides
    virtual void onCreate (MamaSubscription*);
    virtual void onError (MamaSubscription*, const MamaStatus&, const char*);
    virtual void onMsg (MamaSubscription*, MamaMsg&);
    virtual void onQuality (MamaSubscription*, mamaQuality,
                            const char*, short, const void*);

  protected:

    void processInitialUpdate (const MamaMsg &msg,
                               Price_Block *pblock);

    void updatePriceBlock (const MamaMsg &msg,
                           Price_Block *pblock);


  private:
    // This will be used as a Singleton, no copying or copy constructing
    Wombat_Market_Data_Conduit (const Wombat_Market_Data_Conduit&);
    Wombat_Market_Data_Conduit &operator= (const Wombat_Market_Data_Conduit&);

    mamaBridge bridge_;
    Wombat::MamaTransport *transport_;
    Wombat::MamaQueue *queue_;

    typedef std::set <Wombat::MamaSubscription*> SubSet;
    SubSet subscriptions_;

    typedef std::set <std::string> Symbols;
    Symbols symbol_subscriptions_;

    mutable ACE_Thread_Mutex mutex_;

    typedef stdext::hash_map <std::string, Price_Block> Watch_Map;
    typedef std::pair <Watch_Map::iterator, bool> Watch_Map_Insert_Result;
    Watch_Map sym_watches_;

  };

  typedef ACE_Singleton <Wombat_Market_Data_Conduit, ACE_Mutex> WOMBAT_MD_CONDUIT;
}

#endif
