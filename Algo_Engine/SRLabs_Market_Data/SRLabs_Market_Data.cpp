// SRLabs_Market_Data.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include "SRLabs_Market_Data.h"


#ifdef _MANAGED
#pragma managed(push, off)
#endif

BOOL APIENTRY DllMain(HMODULE /* hModule */,
                      DWORD ul_reason_for_call,
                      LPVOID /* lpReserved */)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}

  return TRUE;
}

#ifdef _MANAGED
#pragma managed(pop)
#endif

#include <stdexcept>

class lock_error : public std::runtime_error
{
public:
  explicit lock_error(const std::string &msg)
    : std::runtime_error(msg)
  {
  }
};

class Thread_Mutex
{
public:
  Thread_Mutex()
  {
    if (::InitializeCriticalSectionAndSpinCount(&critsec_,4000))
    {
      const DWORD lasterr=GetLastError();
      char errmsg[128];
      std::string errmsgstr(__FUNCTION__);
      errmsgstr += " calling InitializeCriticalSectionAndSpinCount";
      if (FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM, NULL, lasterr,
                         LANG_SYSTEM_DEFAULT, errmsg, sizeof(errmsg), 0))
      {
        errmsgstr += ": ";
        errmsgstr += errmsg;
      }

      throw lock_error(errmsgstr);
    }
  }

  void Lock()
  {
    EnterCriticalSection(&critsec_);
  }

  void Unlock()
  {
    LeaveCriticalSection(&critsec_);
  }

  virtual ~Thread_Mutex()
  {
    DeleteCriticalSection(&critsec_);
  }

private:
  CRITICAL_SECTION critsec_;
};

class Lock
{
public:
  Lock(Thread_Mutex &mutex)
    : mutex_(mutex)
  {
    this->mutex_.Lock();
  }

  ~Lock()
  {
    this->mutex_.Unlock();
  }

private:
  // Noncopyable and nonassignable
  Lock(const Lock&);
  Lock &operator=(const Lock&);
  Thread_Mutex &mutex_;
};

#include <md-core/mdapplication.h>
#include <md-framework/config/configexception.h>
#include <md-framework/resource/resourceexception.h>
#include <md-core/MDSessionMgr.h>
#include <md-core/MDPriceLevel.h>

#include <iomanip>

md_core::MDApplication *app=0;
md_framework::ConfigRP config;
md_core::MDSessionMgr *cta_sess_mgr = 0;
md_core::MDSessionMgr *utp_sess_mgr = 0;
md_core::MDListener   *cta_listener = 0;
md_core::MDListener   *utp_listener = 0;

// TODO: Plug in error reporting to the ACE framework

SRLABS_MARKET_DATA_API int DART_MD_init(const char *inifile,
                                        const char *section)
{
  int result=-1;
  if (inifile)
  {
    char srlabs_config_file_name[MAX_PATH];
    GetPrivateProfileStringA(
      section,
      "ConfigFile",
      "",
      srlabs_config_file_name,
      sizeof(srlabs_config_file_name),
      inifile);

    if (srlabs_config_file_name[0])
    {
      try
      {
        app = md_core::MDApplication::instance();
        if (app)
        {
          char drive[_MAX_DRIVE];
          char dir[_MAX_DIR];
          char fname[_MAX_FNAME];
          char ext[_MAX_EXT];
          _splitpath(srlabs_config_file_name,drive,dir,fname,ext);
          char path[_MAX_PATH];
          _makepath(path,drive,dir,0,0);
          const size_t len=strlen(path);
          std::string file(fname);
          file+=ext;
          try
          {
            config = app->load(path,file,__MD_VERSION__);
            if (config)
            {
              md_framework::ResourceManager *res_mgr =
                md_core::MDApplication::instance()->getResourceManager();
              if (res_mgr)
              {
                md_framework::Resource *resource =
                  const_cast<md_framework::Resource*>
                    (res_mgr->getResource("MDRCClientSessionMgr","CTA"));
                if (resource)
                {
                  cta_sess_mgr = dynamic_cast<md_core::MDSessionMgr*>(resource);
                  if (cta_sess_mgr)
                  {
                    resource = const_cast<md_framework::Resource*>
                      (res_mgr->getResource("MDRCClientSessionMgr","NASDAQ"));
                    if (resource)
                    {
                      utp_sess_mgr = dynamic_cast<md_core::MDSessionMgr*>(resource);
                      if (utp_sess_mgr)
                        result = 0;
                    }
                  }
                }
              }
            }
          }
          catch (const md_framework::ConfigException &e)
          {
            std::cerr << e.what() << std::endl;
          }
          catch (const md_framework::ResourceException &e)
          {
            std::cerr << e.what() << std::endl;
          }
          catch (...)
          {
            std::cerr << __FUNCTION__ << ": Unknown exception" << std::endl;
          }
        }
      }
      catch (const md_framework::ConfigException &e)
      {
        std::cerr << e.what() << std::endl;
      }
      catch (const md_framework::ResourceException &e)
      {
        std::cerr << e.what() << std::endl;
      }
      catch (...)
      {
        std::cerr << __FUNCTION__ << ": Unknown exception" << std::endl;
      }
    }
  }
  return result;
}

class Listener
  : public md_core::MDListener
{
public:
  virtual ~Listener()
  {
#if DUMP_TO_STDOUT
    std::cout << __FUNCTION__ << std::endl;
#endif
  }

  virtual void onQuote
    (const md_core::MDSubscription* sub,
     const md_core::MDQuote* q)
  {
#if DUMP_TO_STDOUT
    std::cout
      << __FUNCTION__
      << ' ' << std::setw(4) << sub->instrument().symbol() << ' '
      << "Exchange=" << std::setw(1) << q->_oexchange << ' '
      << "Status=" << md_core::MDQuote::toStringStatus(q->_status) << ' '
      << "NBBO=" << q->isNBBO() << ' '
      << "QuoteCondition='" << q->quoteCondition() << "' "
      << "Bid=" << q->_bid << '/' << q->_bidSize << ' '
      << "Ask=" << q->_ask << '/' << q->_askSize << ' '
      << std::endl;
#endif
  }

  virtual void onTrade
    (const md_core::MDSubscription* sub,
     const md_core::MDTrade* t)
  {
#if DUMP_TO_STDOUT
    std::cout << __FUNCTION__ << std::endl;
#endif
  }

  virtual void onOrder
    (const md_core::MDSubscription* sub,
     const md_core::MDOrder* order,
     const md_core::MDTrade* trade)
  {
#if DUMP_TO_STDOUT
    std::cout << __FUNCTION__ << std::endl;
#endif
  }

  virtual void onOrderReplace
    (const md_core::MDSubscription* sub,
     const md_core::MDOrder* oldOrder,
     const md_core::MDOrder* newOrder)
  {
#if DUMP_TO_STDOUT
    std::cout << __FUNCTION__ << std::endl;
#endif
  }

  virtual void onImbalance
    (const md_core::MDSubscription* sub,
     const md_core::MDImbalance* t)
  {
#if DUMP_TO_STDOUT
    std::cout << __FUNCTION__ << std::endl;
#endif
  }

  virtual void onBook
    (const md_core::MDSubscription* sub,
     const md_core::MDOrderBook* book,
     const md_core::MDOrder* cause,
     const md_core::MDTrade* trade)
  {
#if DUMP_TO_STDOUT
    using std::cout;
    using std::endl;
    cout << __FUNCTION__ << endl;
    cout << "Symbol=" << sub->instrument().toString() << endl;
    md_core::MDPriceLevel *pricelevel = book->getBuys();
    cout << "Buy depth=" << pricelevel->getCurrentSize() << endl;
    for (int i(0); i < pricelevel->getCurrentSize(); ++i)
    {
      const md_core::MDPriceLevelInternal *price_level_internal = (*pricelevel)[i];
      cout
        << price_level_internal->_size
        << '@'
        << price_level_internal->_px
        << " on "
        << price_level_internal->_venue
        << endl;
      cout << "\tOrders:";

      typedef std::list<md_core::MDOrder*> Orders;
      for (Orders::const_iterator ord_iter(price_level_internal->_orders->begin());
           ord_iter != price_level_internal->_orders->end(); ++ord_iter)
      {
      }
    }
    cout << endl << endl;
#endif
  }

  virtual void onBookWithOrderReplace
    (const md_core::MDSubscription* sub,
     const md_core::MDOrderBook* book,
     const md_core::MDOrder* oldOrder,
     const md_core::MDOrder* newOrder)
  {
#if DUMP_TO_STDOUT
    std::cout << __FUNCTION__ << std::endl;
#endif
  }

  // Some vendors report only best price and no sizes. User should refer only to prices in the quote and not sizes.
  virtual void onBestPrice
    (const md_core::MDSubscription* sub,
     const md_core::MDQuote* q)
  {
#if DUMP_TO_STDOUT
    std::cout << __FUNCTION__ << std::endl;
#endif
  }

  virtual void onSecurityStatusChange
    (const md_core::InstrumentDef* instrument)
  {
#if DUMP_TO_STDOUT
    std::cout << __FUNCTION__ << std::endl;
#endif
  }

  virtual void onSubscriptionInvalid
    (const md_core::MDSubscription *sub,
     const char *reason)
  {
#if DUMP_TO_STDOUT
    std::cout << __FUNCTION__ << std::endl;
#endif
  }

  // Alerts: Fired only by NyseAlerets feed
  virtual void onTradingIndicationAlert
    (const md_core::MDSubscription *sub,
     const md_core::MDMktTradingIndicationAlert* alert)
  {
#if DUMP_TO_STDOUT
    std::cout << __FUNCTION__ << std::endl;
#endif
  }

  virtual void onTradeDisseminationTimeAlert
    (const md_core::MDSubscription *sub,
     const md_core::MDMktTradeDisseminationTimeAlert* alert)
  {
#if DUMP_TO_STDOUT
    std::cout << __FUNCTION__ << std::endl;
#endif
  }

  // CB's that cannot be normalized
  virtual void onCustomEvent
    (const md_core::MDSubscription *sub,
     const md_core::MDCustomEvent* event)
  {
#if DUMP_TO_STDOUT
    std::cout << __FUNCTION__ << std::endl;
#endif
  }
};

typedef std::pair <md_core::InstrumentDef,Price_Block*> SymInfo;
typedef std::map<std::string,SymInfo> Instruments;
Instruments instruments;

SRLABS_MARKET_DATA_API int DART_MD_subscribe(const char *symbol)
{
  int result(-1);
  if (symbol && cta_sess_mgr)
  {
    md_core::InstrumentDef instrdef
      (md_core::InstrumentDef::EXCH_STOCK,"",symbol,"");
    Price_Block *pb=new Price_Block;
    instruments[symbol]=std::make_pair(instrdef,pb);
    pb->ask_price=0.0;
    pb->ask_size=0;
    pb->bid_price=0.0;
    pb->bid_size=0;
    if (cta_listener==0)
      cta_listener = new Listener;
    md_core::MDSubscription *subscription =
      cta_sess_mgr->subscribe(instrdef,cta_listener);
    if (subscription)
    {
      cta_sess_mgr->appNotify();
      cta_sess_mgr->start();
      result = 0;
    }
    if (utp_listener==0)
      utp_listener = new Listener;
    subscription = utp_sess_mgr->subscribe(instrdef,utp_listener);
    if (subscription)
    {
      utp_sess_mgr->appNotify();
      utp_sess_mgr->start();
      result = 0;
    }
  }
  return result;
}

SRLABS_MARKET_DATA_API int DART_MD_unsubscribe(const char *symbol)
{
  int result(-1);
  Instruments::iterator instr_iter=instruments.find(symbol);
  if (instr_iter!=instruments.end())
  {
    cta_sess_mgr->unsubscribe(instr_iter->second.first);
    utp_sess_mgr->unsubscribe(instr_iter->second.first);
    instruments.erase(instr_iter);
    return 0;
  }
  return result;
}

bool find_quote (md_core::MDSessionMgr *sessmgr,
                 const md_core::InstrumentDef &instrdef,
                 md_core::MDQuote *quote,
                 Price_Block *pb)
{
  bool result(false);
  if (sessmgr->findQuote(instrdef,quote))
  {
    pb->ask_price=quote->_ask;
    pb->bid_price=quote->_bid;
    pb->ask_size=quote->_askSize;
    pb->bid_size=quote->_bidSize;
    strncpy (pb->bid_participant_id,
      quote->_bexchange,
      sizeof(pb->bid_participant_id));
    pb->bid_participant_id [sizeof(pb->bid_participant_id)-1] = '\0';
    strncpy (pb->ask_participant_id,
      quote->_oexchange,
      sizeof(pb->ask_participant_id));
    pb->ask_participant_id [sizeof(pb->ask_participant_id)-1] = '\0';
    pb->quote_seq_no = quote->_seqno;
    result=true;
  }
  return result;
}

SRLABS_MARKET_DATA_API int DART_MD_get_price_block(const char *symbol,
                                                   Price_Block *pb)
{
  int result=-1;
  if (symbol && pb)
  {
    Instruments::const_iterator instr_iter=instruments.find(symbol);
    if (instr_iter!=instruments.end())
    {
      const md_core::InstrumentDef &instr=instr_iter->second.first;
      md_core::MDQuote *quote=new md_core::MDQuote;
      if (find_quote (cta_sess_mgr,instr,quote,pb)
          || find_quote (utp_sess_mgr,instr,quote,pb))
        result=0;
    }
  }
  return result;
}

SRLABS_MARKET_DATA_API int DART_MD_init_price_block(Price_Block *pblock)
{
  pblock->bid_price=0.0;
  pblock->ask_price=0.0;
  pblock->bid_size=-1;
  pblock->ask_size=-1;
  pblock->lot_size=100;
  pblock->quote_seq_no=-1;
  pblock->initialized=1;
  memset (pblock->bid_participant_id,0,sizeof(pblock->bid_participant_id));
  memset (pblock->ask_participant_id,0,sizeof(pblock->ask_participant_id));
  return 0;
}

SRLABS_MARKET_DATA_API void DART_MD_shutdown()
{
  cta_sess_mgr->stop();
  utp_sess_mgr->stop();
  md_core::MDApplication::instance()->getResourceManager()->shutdown();
}