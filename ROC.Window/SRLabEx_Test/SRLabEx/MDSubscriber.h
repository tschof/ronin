#ifndef __MDSubscriber_H__
#define __MDSubscriber_H__

#include "md-framework/util/MDCommon.h"
#include "md-core/MDApplication.h"
#include "md-core/MDSessionMgr.h"
#include "md-core/InstrumentDef.h"
#include "md-core/MDListener.h"
#include "md-core/MDAdminListener.h"
#include "md-core/MDAlerts.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>
#include <vector>
#include <iostream>

namespace md_test
{
	struct LResult
	{
		unsigned long long _sequence;
		long long _latency;
		md_core::MDSubscription * _sub;
		LResult()
		{
			_sequence = 0;
			_latency = -1;
			_sub = 0;
		}
	};
class MDSubscriber : public  md_core::MDListener, public md_core::MDAdminListener
{
public:
	MDSubscriber();
	virtual ~MDSubscriber()
	{
	}
	virtual void init() throw (md_framework::ConfigException,md_framework::ResourceException);
	void start();
   void onBookWithOrderReplace (const md_core::MDSubscription* sub, const md_core::MDOrderBook* book,const md_core::MDOrder* oldorder, const md_core::MDOrder* neworder);
	void onBook (const md_core::MDSubscription* sub, const md_core::MDOrderBook* book,const md_core::MDOrder* cause, const md_core::MDTrade * trade);
   void onOrderReplace (const md_core::MDSubscription *sub, const md_core::MDOrder *oldOrder, const md_core::MDOrder *newOrder);
   void onOrder (const md_core::MDSubscription *sub, const md_core::MDOrder *o, const md_core::MDTrade *trade);
   void onQuote (const md_core::MDSubscription* sub,const md_core::MDQuote* q);
   void onTrade (const md_core::MDSubscription* sub,const md_core::MDTrade* t);
   void onImbalance (const md_core::MDSubscription *sub, const md_core::MDImbalance *i);
   void onBestPrice (const md_core::MDSubscription* sub,const md_core::MDQuote* q);
   void onCustomEvent(const md_core::MDSubscription* sub, const md_core::MDCustomEvent* event);
   void onSecurityStatusChange(const md_core::InstrumentDef* instrument);
	void onSubscriptionInvalid (const md_core::MDSubscription *sub, const char *reason);
	//Alerts
   void onTradingIndicationAlert (const md_core::MDSubscription *sub,const md_core::MDMktTradingIndicationAlert* alert);
   void onTradeDisseminationTimeAlert (const md_core::MDSubscription *sub,const md_core::MDMktTradeDisseminationTimeAlert* alert);
	//Admin Listener
	void onPlaybackComplete(md_core::MDSessionMgr* sm);
	void onGap (md_core::MDSessionMgr* sm,const char* connection,const char* line, unsigned long long prevSNO, unsigned long long curSNO,bool isRecoveryEnabled);
   void onGapFill(md_core::MDSessionMgr* sm,const char* connection,const char* line, unsigned long long fromSNO, unsigned long long curSNO, bool isFullyFilled);
	void onSlowConsumer(md_core::MDSessionMgr* sm,const char* connection,const char* line, int numEventsSampled,int numEventsAboveThreshold,
                       long long maxUserTimeInMicros, long long minUserTimeInMicros);
	void onMDRCForcedDisconnect(md_core::MDSessionMgr*, const char* connection,const char* line, const char* reason);


	void shutdown();
	bool parseCmdLine(int ac, char** av);
	static void sigH(int signum);
private:
  void calculateMetrics(const md_core::MDSubscription *sub, const md_core::MDBaseData * basedata);
  void calculateStatistics();
  void printTradeDissemination(const md_core::MDSubscription* sub, const md_core::MDMktTradeDisseminationTimeAlert* alert);
  void printTradeIndication(const md_core::MDSubscription* sub, const md_core::MDMktTradingIndicationAlert* alert);
  void printProduct(const md_core::MDSubscription* sub, const md_core::MDBaseData* basedata, const md_core::MDProductInfo* product);
  void printCustom(const md_core::MDSubscription* sub, const md_core::MDCustomEvent* event);
  void printBook(const md_core::MDSubscription* sub, const md_core::MDOrderBook* book);
  void printTrade(const md_core::MDSubscription* sub, const md_core::MDTrade* trade);
  void printSecurityStatus(const md_core::MDSubscription* sub, const md_core::InstrumentDef* instrument);
  void printQuote(const md_core::MDSubscription* sub, const md_core::MDQuote* quote, char qtype);
  void printOrder(const md_core::MDSubscription* sub, const md_core::MDOrder* order);
  void printImbalance(const md_core::MDSubscription* sub, const md_core::MDImbalance* imbalance);
  bool printHeader(char type, const md_core::MDSubscription* sub, const md_core::MDBaseData* base, const md_core::InstrumentDef * instrument);
  void checkQuote(const md_core::MDSubscription* sub, const md_core::MDQuote* quote);
  void checkTrade(const md_core::MDSubscription* sub, const md_core::MDTrade* trade);
  void checkBook(const md_core::MDSubscription* sub, const md_core::MDOrderBook* book);
  void checkOrder(const md_core::MDSubscription* sub, const md_core::MDOrder* order);

  static char printChar(char in, char def)
  {
	  return in ? in : def;
  }

	bool _shutdown;
	bool _mainshutdown;
	FILE * _csvfile;
#ifndef __GNUATOMICOPS__
	md_framework::SpinMutex _mmutex;
#endif
   md_framework::SimpleMutex _writefilemutex; // for writing to csv
	int  _logct;
	char _timebuffer[20];
	std::string _symbolfile;
	std::string _symbolnames;
	bool _dqcheck;
	std::vector<std::pair<md_core::InstrumentDef,std::string> >* _instruments;
	md_core::MDApplication* _app;
	std::vector<md_core::MDSessionMgr*> _mdSMList;
	md_framework::ConfigRP _config;
	md_framework::ILogger* _logger;
	std::vector<std::pair<std::string,std::string> > _typeNameList;
	static int _sigct;
	static MDSubscriber* _gHandle;
   unsigned int _maxEvents;
	unsigned int _eventCount;
   unsigned int _pctCount;
	int _latCount;
	int _printDepth;
	int _smCount;
	bool _autoload;

	LResult * _latencyResults;
	LResult * _mdrcLatencyResults;
};
}

#endif
