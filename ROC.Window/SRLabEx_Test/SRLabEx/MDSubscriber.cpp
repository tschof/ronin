#include "md-framework/util/MDCommon.h"
#include <assert.h>
#include <time.h>

#ifdef _WINDOWS
#ifndef __MD_VERSION__
#error __MD_VERSION__ MUST BE BUILT FOR WINDOWS
#endif
#ifndef ZLIB_WINAPI
#error ZLIB_WINAPI MUST BE BUILT FOR WINDOWS
#endif
#ifndef __MDRC_ENABLED__
#error __MDRC_ENABLED__ MUST BE BUILT FOR WINDOWS
#endif
#include <process.h>
#include <io.h>
#include <fcntl.h>
#include <share.h>
#include "getopt.h"
#else
#include <strings.h>
#include <sys/sysinfo.h>
#include <sys/resource.h>
#include <sys/time.h>
#endif

#include <math.h>
#include "MDSubscriber.h"
#include "md-framework/util/StringUtil.h"
#include "md-framework/util/TimeOffsetUtil.h"
#include "md-framework/util/MDTime.h"
#include "md-framework/admin/AdminMgr.h"

using namespace std;
using namespace md_framework;
using namespace md_core;
using namespace md_test;

MDSubscriber* MDSubscriber::_gHandle = 0;
int MDSubscriber::_sigct = 0;

void MDSubscriber::init() throw (ConfigException, ResourceException)
{
	//initialize all required Subscriptions
	if (!_symbolnames.empty())
	{
		std::vector<std::string> syms;
		StringUtil::tokenize(syms, _symbolnames, ',');
		std::vector<std::string>::iterator i = syms.begin();
		while (i != syms.end())
		{
			char buf[128];
			strncpy(buf, (*i).c_str(), sizeof(buf));
			++i;
#ifdef _WINDOWS
			char * exchange = strrchr(buf, '.');
#else
			char * exchange = rindex(buf, '.');
#endif
         if (exchange)
         {
            (*exchange) = 0; // null terminate in the middle
         }
			InstrumentDef stock(InstrumentDef::EXCH_UNKNOWN, "", buf, exchange ? exchange + 1 : 0);
			cerr << "Command line subscribing to " << stock.key() << endl;
			_instruments->push_back(std::pair<InstrumentDef,std::string>(stock,""));
		}
	}
	else if (!_symbolfile.empty())
	{
		cerr << "Reading symbols from " << _symbolfile << endl;
		FILE* fp = fopen(_symbolfile.c_str(), "r");

		if (!fp)
		{
			printf("Unable to open file %s\n", _symbolfile.c_str());
			std::string err = "Unable to open file";
			throw ConfigException(err);
		}
		char buf[512];
		while (fgets(buf, sizeof(buf), fp))
		{
			if (buf[strlen(buf) - 1] == '\n')
			{
				buf[strlen(buf) - 1] = 0;
			}

			// set symbol / exchange correctly
#ifdef _WINDOWS
			char * exchange = strrchr(buf, '.');
#else
			char * exchange = rindex(buf, '.');
#endif
			if (exchange)
			{
				(*exchange) = 0; // null terminate in the middle
			}
			InstrumentDef stock(InstrumentDef::EXCH_UNKNOWN, "", buf, exchange ? exchange + 1 : 0);
			//if (exchange) (*exchange) = '.';  // put it back to print out
			//cerr << "Read " << buf << " into " << stock.toString() << " s " << stock.symbol() << " e " << stock.exchange() << endl;

			_instruments->push_back(std::pair<InstrumentDef,std::string>(stock,""));
		}
		fclose(fp);
	}
	else
	{
		const ConfigRP& subC = _config->getFirstChildByName("SubscriptionList");
		if (subC)
		{
			ConfigList cl;
			subC->getChildrenByName("Symbol", cl);
			ConfigListIterator i = cl.begin();
			while (i != cl.end())
			{
				const ConfigRP& sym = *i++;
				const std::string& name = sym->getAttrValue("name");
				std::string suffix = sym->getAttrValue("suffix");
				std::string smname = sym->getAttrValue("smname");
				InstrumentDef stock(InstrumentDef::EXCH_UNKNOWN, "", name.c_str(), suffix.c_str());
				_instruments->push_back(std::pair<InstrumentDef,std::string>(stock,smname));
			}
		}
	}

	if (_autoload)
	{
		const ConfigRP& subC = _config->getFirstChildByName("Resources");
		ConfigList cl;
		subC->getChildrenByName("Resource", cl);
		ConfigListIterator i = cl.begin();
		while (i != cl.end())
		{
			const ConfigRP& rsc = *i++;
			string stype = rsc->getAttrValue("type");
			string sname = rsc->getAttrValue("name");
			if (!stype.empty() && !sname.empty() && stype.find("SessionMgr") != string::npos)
			{	
				cerr << "Autoload resource " << stype << " / " << sname << endl;
				_typeNameList.push_back(std::pair<std::string, std::string>(stype, sname));
			}
		}
	}

	//Load MDSessionMgr from MDApplication
	std::vector<std::pair<std::string, std::string> >::iterator tnI = _typeNameList.begin();
	while (tnI != _typeNameList.end())
	{
		const std::pair<std::string, std::string>& tnPair = *tnI++;
		const Resource* r = _app->getResourceManager()->getResource(tnPair.first.c_str(), tnPair.second.c_str());
		if (!r)
		{
			std::string m = "MDSubscriber::init: No resource defined for type=";
			m.append(tnPair.first).append(", name=").append(tnPair.second);
			throw ConfigException(m);
		}
		MDSessionMgr* mdSM = dynamic_cast<MDSessionMgr*> (const_cast<Resource*> (r));
		if (!mdSM && !_autoload)
		{
			std::string m = "MDSubscriber::init: Resource for type=";
			m.append(tnPair.first).append(", name=").append(tnPair.second).append(" should be of Type MDSessionMgr");
			throw ConfigException(m);
		}
		if (mdSM)
		{
			_mdSMList.push_back(mdSM);
		}
	}
}

void MDSubscriber::start()
{
	cerr << "Subscribing to " << _instruments->size() << " instruments..";
	cerr.flush();
	std::vector<std::pair<InstrumentDef,std::string> >::iterator i = _instruments->begin();
	_smCount = _mdSMList.size();

	// NOTICE ! Modified From Down
	//std::vector<md_core::MDSessionMgr*>::iterator j = _mdSMList.begin();
	//while (j != _mdSMList.end())
	//{
	//	MDSessionMgr* mdSM = *j++;
	//	mdSM->registerAdminListener(this);
	//	mdSM->appNotify();
	//	if (mdSM->getAdminMgr())
	//	{
	//		mdSM->getAdminMgr()->appNotify();
	//	}
	//}

	while (i != _instruments->end())
	{
		try
		{
			InstrumentDef& ins = i->first;
			std::string& smname = i->second;
			i++;
			std::vector<md_core::MDSessionMgr*>::iterator j = _mdSMList.begin();
			while (j != _mdSMList.end())
			{
				MDSessionMgr* mdSM = *j++;
				ins.source(mdSM->getName().c_str(), mdSM->getName().length());
				if ( smname.empty() || !strcmp(mdSM->getName().c_str(),smname.c_str()))
				{
					mdSM->subscribe(ins, this);
				}
			}
		}
		catch (const MDException& mde)
		{
			cerr << mde.what() << endl;
		}
	}

	// NOTICE ! Moved UP
	std::vector<md_core::MDSessionMgr*>::iterator j = _mdSMList.begin();
	while (j != _mdSMList.end())
	{
		MDSessionMgr* mdSM = *j++;
		mdSM->registerAdminListener(this);
		mdSM->appNotify();
		if (mdSM->getAdminMgr())
		{
			mdSM->getAdminMgr()->appNotify();
		}
	}
	cerr << "done" << endl;
	//AppNotfiy All Resources. Dont notfiy all resources. Notify only ones that user wants. Done above in while loop.
	//The below can be done if User wants to blindly start everything.
	//_app->getResourceManager()->appNotify();
	while (!_mainshutdown)
	{
		struct timeval to =
		{ 5, 0 };
#ifdef _WINDOWS
		Sleep(to.tv_sec*1000);
#else
		select(0,0,0,0,&to);
#endif
	}
}

void MDSubscriber::shutdown()
{
	{
		SimpleLock lk(_writefilemutex);
		if (_shutdown) return;
		_shutdown = true;
		if (_csvfile)
		{
			fclose(_csvfile);
			_csvfile=NULL;
		}
	}
	cerr << "Shutting down. unsubscribing..";
	cerr.flush();
	if (!_mdSMList.empty())
	{
		cerr << "Unsubscribing " << _instruments->size() << " instruments." << endl;
		std::vector<std::pair<InstrumentDef,std::string> >::iterator i = _instruments->begin();
		while (i != _instruments->end())
		{
			InstrumentDef& ins = i->first;
			std::string& smname = i->second;
			i++;
			std::vector<md_core::MDSessionMgr*>::iterator j = _mdSMList.begin();
			while (j != _mdSMList.end())
			{
				MDSessionMgr* mdSM = *j++;
				mdSM->unsubscribe(ins);
				if ( smname.empty() || !strcmp(mdSM->getName().c_str(),smname.c_str()))
				{
					mdSM->unsubscribe(ins);
				}
				//cerr << "Unsubscribed "<<mdSM->getType()<<":"<<mdSM->getName()<<"["<<ins.key()<<"]"<<endl;
			}
		}
	}
	std::vector<md_core::MDSessionMgr*>::iterator j = _mdSMList.begin();
	while (j != _mdSMList.end())
	{
		MDSessionMgr* mdSM = *j++;
		mdSM->stop();
	}
	cerr << "done." << endl;

	// kill everything else
	_app->getResourceManager()->shutdown();

	if (_app->inMetricsMode()) calculateStatistics();
	cerr << "Flushing Log file." <<endl;
	if (_logger)
	{
		_logger->stop();
	}
	cerr << "Finished Flushing Log file." <<endl;
	_mainshutdown = true;
}

void MDSubscriber::onPlaybackComplete(md_core::MDSessionMgr* sm)
{
	_logger->logV(MD_INFO_MSG,"Playback completed for MDSessionMgr = %s" , sm->getName().c_str());
	cerr << "Playback completed for MDSessionMgr = " << sm->getName().c_str() << endl;
	--_smCount;
	if (_smCount <= 0)
	{
		cerr << "Playback complete for all session managers, exiting automatically" << endl;
		shutdown();
	}
}
void MDSubscriber::onGap(md_core::MDSessionMgr* sm, const char* connection, const char* line, unsigned long long prevSNO, unsigned long long curSNO, bool isRecoveryEnabled)
{
#if ( (_WINDOWS) && (_MSC_VER < 1400) )
	_logger->logV(MD_WARN_MSG,"Gap detect for[%s:%s:%s]. Range=[%lu,%lu]" , sm->getName().c_str(), connection, line, (unsigned long)prevSNO, (unsigned long)curSNO);
#else
	_logger->logV(MD_WARN_MSG,"Gap detect for[%s:%s:%s]. Range=[%llu,%llu]" , sm->getName().c_str(), connection, line, prevSNO, curSNO);
#endif
}
void MDSubscriber::onGapFill(md_core::MDSessionMgr* sm, const char* connection, const char* line, unsigned long long fromSNO, unsigned long long curSNO, bool isFullyFilled)
{

#if ( (_WINDOWS) && (_MSC_VER < 1400) )
	_logger->logV(MD_WARN_MSG,"Gap Fill for[%s:%s:%s]. Range=[%lu,%lu]. Fully Filled=%s" , sm->getName().c_str(), connection, line, (unsigned long)fromSNO, (unsigned long)curSNO,isFullyFilled ? "YES" : "NO");
#else
	_logger->logV(MD_WARN_MSG,"Gap Fill for[%s:%s:%s]. Range=[%llu,%llu]. Fully Filled=%s" , sm->getName().c_str(), connection, line, fromSNO, curSNO,isFullyFilled ? "YES" : "NO");

#endif
	
}
void MDSubscriber::onSlowConsumer(MDSessionMgr* sm, const char* connection, const char* line, int numEventsSampled, int numEventsAboveThreshold, long long maxUserTimeInMicros,
		long long minUserTimeInMicros)
{
	_logger->logV(MD_WARN_MSG,"Slow consumer warning from SMDS for SessionMgr[%s,%s:%s],Number of events sampled = %d,Number of Events above threshold=%d, Max user Time in Micros=%lld" ,
	sm->getName().c_str(), connection, line, numEventsSampled, numEventsAboveThreshold, maxUserTimeInMicros);

}

void MDSubscriber::onMDRCForcedDisconnect(MDSessionMgr*, const char* connection,const char* line, const char* reason)
{
}

int compareLR(const void* a, const void* b)
{
	return ((LResult*) a)->_latency - ((LResult*) b)->_latency;
}

void MDSubscriber::calculateStatistics()
{
	if (_latencyResults == 0)
	{
		cerr << "Unable to calculate latency as maxEvents was not specified" << endl;
		return;
	}
	cerr << "Calculating latency results from " << _maxEvents << " events:" << flush;
	cerr << " logging" << flush;
	bool hasmdrc = false;
	double mean = 0.0;
	double mdrcmean = 0.0;
	double stdev, mdrcstdev;
	stdev = mdrcstdev = 0;
	unsigned int eventcount = _maxEvents;
	for (unsigned int i = 0; i < _maxEvents; i++)
	{
		if (_latencyResults[i]._sub == 0)
		{
			cerr << endl << "WARNING: 0 latency result at event " << (i + 1) << " starting latency calculation at this point" << endl;
			eventcount = i;
			break;
		}
		mean += _latencyResults[i]._latency;
#if ( (_WINDOWS) && (_MSC_VER < 1400) )
		_logger->logV(MD_INFO_MSG,"SOURCE: %s INS:%s SQNO:%lu LATENCY:%ld" , _latencyResults[i]._sub->instrument().source(), _latencyResults[i]._sub->instrument().key(), (unsigned long)_latencyResults[i]._sequence, (long)_latencyResults[i]._latency);
#else
		_logger->logV(MD_INFO_MSG,"SOURCE: %s INS:%s SQNO:%llu LATENCY:%lld" , _latencyResults[i]._sub->instrument().source(), _latencyResults[i]._sub->instrument().key(), _latencyResults[i]._sequence,  _latencyResults[i]._latency);
#endif

		if (_mdrcLatencyResults && (hasmdrc || _mdrcLatencyResults[i]._sub))
		{
			hasmdrc = true;
			mdrcmean+=_mdrcLatencyResults[i]._latency;
#if ( (_WINDOWS) && (_MSC_VER < 1400) )
			_logger->logV(MD_INFO_MSG, "MDRCSRC: %s MDRCINS:%s MDRCSQNO:%lu MDRCLATENCY:%ld", _latencyResults[i]._sub->instrument().source(), _mdrcLatencyResults[i]._sub->instrument().key(), (unsigned long)_mdrcLatencyResults[i]._sequence, (long)_mdrcLatencyResults[i]._latency);
#else
			_logger->logV(MD_INFO_MSG, "MDRCSRC: %s MDRCINS:%s MDRCSQNO:%llu MDRCLATENCY:%lld", _latencyResults[i]._sub->instrument().source(), _mdrcLatencyResults[i]._sub->instrument().key(), _mdrcLatencyResults[i]._sequence, _mdrcLatencyResults[i]._latency);
#endif
		}
	}
	if (eventcount == 0)
	{
		cerr << "WARNING: No events to calculate, no latency result possible" << endl;
		return;
	}
	mean = mean / (double)eventcount;
	if (hasmdrc) mdrcmean = mdrcmean / (double)eventcount;
	cerr << " stddev" << flush;

	for (unsigned int i = 0; i < eventcount; i++)
	{
		double t = (double)_latencyResults[i]._latency-mean;
		stdev += (t*t);
		if (hasmdrc)
		{
			t = (double)_mdrcLatencyResults[i]._latency-mdrcmean;
			mdrcstdev += (t*t);
		}
	}
	if (eventcount > 1)
	{
		stdev = stdev/(eventcount-1);
		if (hasmdrc) mdrcstdev = mdrcstdev/(eventcount-1);
	}
	else
	{
		stdev = mdrcstdev = 0;
	}
	stdev=sqrt(stdev);
	if (hasmdrc) mdrcstdev = sqrt(stdev);

	cerr << " sorting" << flush;
	qsort(_latencyResults, eventcount, sizeof(LResult), compareLR);
	if (hasmdrc) qsort(_mdrcLatencyResults, eventcount, sizeof(LResult), compareLR);
	cerr << " displaying:" << endl;
	int percentiles [] =
	{	50, 70, 75, 80, 85, 90, 91, 92, 93, 94, 95, 96, 97, 98, 99};
	int psize = sizeof(percentiles)/sizeof(int);
	if (hasmdrc)
	{
		printf("Samples: %u, Mean: %.2fus, STDEV: %.2f, MDRC Mean: %.2fus, MDRCSTDEV: %.2f\n", eventcount, mean, stdev, mdrcmean, mdrcstdev);
	}
	else
	{
		printf("Samples: %u, Mean: %.2fus, STDEV: %.2f\n", eventcount, mean, stdev);
	}
	for (int i = 0; i < psize; i++)
	{
		printf("%7d", percentiles[i]);
		printf("%%");
	}
	printf("\n");
	for (int i = 0; i < psize; i++)
	{
		long long pos = percentiles[i] * eventcount;
		pos = pos / 100;
		if (pos >= eventcount) pos = eventcount - 1;
		printf("%8lld", _latencyResults[pos]._latency);
	}
	printf("\n");

	if (!hasmdrc) return;
	for (int i = 0; i < psize; i++)
	{
		long long pos = percentiles[i] * eventcount;
		pos = pos / 100;
		if (pos >= eventcount) pos = eventcount - 1;
		printf("%8lld", _mdrcLatencyResults[pos]._latency);
	}
	printf("\n");
}

void MDSubscriber::calculateMetrics(const MDSubscription *sub, const MDBaseData * basedata)
{
	bool takeevent = false;
	int ecnt=0;
	{
#ifdef __GNUATOMICOPS__
		int lc = __sync_add_and_fetch(&_logct, 1);
		if (lc % _latCount == 0)
		{
			takeevent = true;
			ecnt = __sync_add_and_fetch(&_eventCount, 1);
		}
#else
		SpinLock lk(_mmutex);
		if ((++_logct % _latCount) == 0)
		{
			takeevent = true;
			ecnt = ++_eventCount;
		}
#endif
	}
	if (takeevent)
	{
		if (_maxEvents > 0)
		{
			if (ecnt > _maxEvents)
			{
				return;
			}
			if (ecnt % _pctCount == 0)
			{
				cerr << ((100 * ecnt) / _maxEvents) << "% ";
				cerr.flush();
			}
		}
		const timeval& tv = sub->usertime();
		long long latency = 0;
		unsigned long long startMS = (long long) basedata->_recvTime.tv_sec * 1000000ULL + (unsigned long long) basedata->_recvTime.tv_usec;
		unsigned long long endMS = (unsigned long long) tv.tv_sec * 1000000ULL + (unsigned long long) tv.tv_usec;
		latency = (endMS - startMS);
		if (latency <= 0)
		{
			_logger->logV(MD_WARN_MSG,"Negative or Zero latency: SM:%s, SYMBOL:%s LATENCY (us):%lld" , sub->getSM()->getName().c_str(), sub->instrument().symbol(),
			latency);
			return;
		}
		if (_latencyResults!=0)
		{
			if (_logger->isDebugEnabled())
			{
				_logger->logV(MD_DEBUG_MSG, "SM:%s, SYMBOL:%s LATENCY (us):%2lld", sub->getSM()->getName().c_str(), sub->instrument().symbol(), latency);
			}
			_latencyResults[ecnt-1]._latency = latency;
			_latencyResults[ecnt-1]._sub = (MDSubscription*)sub;
			_latencyResults[ecnt-1]._sequence = basedata->_originalseqno;
		}
		else
		{
			if (_logger->isInfoEnabled())
			{
				_logger->logV(MD_INFO_MSG, "SM:%s, SYMBOL:%s LATENCY (us):%2lld", sub->getSM()->getName().c_str(), sub->instrument().symbol(), latency);
			}
		}
		if (basedata->_mdrcRecvTime.tv_sec > 0)
		{
			unsigned long long endMDRC = (unsigned long long)basedata->_mdrcRecvTime.tv_sec * 1000000ULL + (unsigned long long)basedata->_mdrcRecvTime.tv_usec;
			latency = (endMS - endMDRC);
			if (latency <= 0)
			{
				_logger->logV(MD_WARN_MSG, "Negative or Zero latency: SM:%s, SYMBOL:%s LATENCY (us):%2lld", sub->getSM()->getName().c_str(),
						sub->instrument().symbol(), latency);
				return;
			}

			if (_mdrcLatencyResults!=0)
			{
				if (_logger->isDebugEnabled())
				{
					_logger->logV(MD_DEBUG_MSG, "SM:%s, SYMBOL:%s MDRCLTNCY (us):%2lld", sub->getSM()->getName().c_str(), sub->instrument().symbol(), latency);
				}
				_mdrcLatencyResults[ecnt-1]._latency = latency;
				_mdrcLatencyResults[ecnt-1]._sub = (MDSubscription*)sub;
				_mdrcLatencyResults[ecnt-1]._sequence = basedata->_originalseqno;
			}
			else
			{
				if (_logger->isInfoEnabled())
				{
					_logger->logV(MD_INFO_MSG, "SM:%s, SYMBOL:%s MDRCLTNCY (us):%2lld", sub->getSM()->getName().c_str(), sub->instrument().symbol(), latency);
				}
			}
		}
		if (_maxEvents > 0)
		{
			if (ecnt == _maxEvents)
			{
				cerr << "Counted " << ecnt << " events, terminating " << endl;
				shutdown();
				return;
			}
		}
	}
}

void MDSubscriber::onBookWithOrderReplace(const MDSubscription* sub, const MDOrderBook* book, const MDOrder* oldorder, const MDOrder* neworder)
{
	if (_app->inMetricsMode())
	{
		calculateMetrics(sub, book);
		return;
	}
	if (_logger->isDebugEnabled())
	{
		_logger->logV(MD_DEBUG_MSG,"NEW Book [%s] for instrument[%s]" , const_cast<MDOrderBook*> (book)->toString(), sub->instrument().toString());
		if (oldorder)
		{
			_logger->logV(MD_DEBUG_MSG,"Book Changed by Delete of Order %s", const_cast<MDOrder*> (oldorder)->toString());
		}
		if (neworder)
		{
			_logger->logV(MD_DEBUG_MSG,"Book Changed by Add of Order %s", const_cast<MDOrder*> (neworder)->toString());
		}
	}
	printBook(sub, book);
	checkBook(sub, book);
	if (oldorder) printOrder(sub, oldorder);
	if (oldorder) checkOrder(sub, oldorder);
	if (neworder) printOrder(sub, neworder);
	if (neworder) checkOrder(sub, neworder);
	printProduct(sub, book, sub->product());

}
void MDSubscriber::onBook(const MDSubscription* sub, const MDOrderBook* book, const MDOrder* cause, const MDTrade* t)
{
	if (_app->inMetricsMode())
	{
		calculateMetrics(sub, book);
		return;
	}
	if (_logger->isDebugEnabled())
	{
		_logger->logV(MD_DEBUG_MSG,"NEW Book [%s] for instrument[%s]" , const_cast<MDOrderBook*> (book)->toString(), sub->instrument().toString());
		if (cause)
		{
			_logger->logV(MD_DEBUG_MSG,"Book Changed by Order %s", const_cast<MDOrder*> (cause)->toString());
		}
		if (t)
		{
			_logger->logV(MD_DEBUG_MSG,"Book Changed by Trade[%s] for instrument[%s]", const_cast<MDTrade *> (t)->toString(), sub->instrument().toString());
		}

	}
	printBook(sub, book);
	checkBook(sub, book);
	if (cause) printOrder(sub, cause);
	if (cause) checkOrder(sub, cause);
	if (t) printTrade(sub, t);
	if (t) checkTrade(sub, t);

	printProduct(sub, book, sub->product());
}

void MDSubscriber::onTradingIndicationAlert(const MDSubscription *sub, const MDMktTradingIndicationAlert* alert)
{
	if (_logger->isDebugEnabled())
	{
		_logger->logV(MD_DEBUG_MSG,"onTradingIndicationAlert [%s] for instrument[%s]" , const_cast<MDMktTradingIndicationAlert*> (alert)->toString(),
		sub->instrument().toString());
	}
	printTradeIndication(sub, alert);
}


void MDSubscriber::onCustomEvent(const md_core::MDSubscription* sub, const md_core::MDCustomEvent* event)
{
	//cerr << "OnCustom received" << endl; 
	if (_logger->isDebugEnabled())
	{
		_logger->logV(MD_DEBUG_MSG,"onCustomEvent [%s] for instrument[%s]" , const_cast<MDCustomEvent*> (event)->toString(), sub->instrument().toString());
	}
	if (sub->userData() == 0)
	{
		sub->userData(_app);
	}
	printCustom(sub, event);
}

void MDSubscriber::onTradeDisseminationTimeAlert(const MDSubscription *sub, const MDMktTradeDisseminationTimeAlert* alert)
{
	if (_logger->isDebugEnabled())
	{
		_logger->logV(MD_DEBUG_MSG,"onTradeDisseminationTimeAlert [%s] for instrument[%s]" , const_cast<MDMktTradeDisseminationTimeAlert*> (alert)->toString(),
		sub->instrument().toString());
	}
	printTradeDissemination(sub, alert);
}

void MDSubscriber::onQuote(const MDSubscription* sub, const MDQuote* q)
{
	if (_app->inMetricsMode())
	{
		calculateMetrics(sub, q);
		return;
	}
	if (_logger->isDebugEnabled())
	{
		_logger->logV(MD_DEBUG_MSG,"NEW Quote[%s] for instrument[%s]" , const_cast<MDQuote *> (q)->toString(), sub->instrument().toString());
	}
	printQuote(sub, q, 'Q');
	checkQuote(sub, q);
	printProduct(sub, q, sub->product());
}

void MDSubscriber::onBestPrice(const MDSubscription* sub, const MDQuote* q)
{
	if (_app->inMetricsMode())
	{
		calculateMetrics(sub, q);
		return;
	}
	if (_logger->isDebugEnabled())
	{
		_logger->logV(MD_DEBUG_MSG,"NEW Best Price[%s] for instrument[%s]" , const_cast<MDQuote*> (q)->toString(), sub->instrument().toString());
	}
	printQuote(sub, q, 'B');
	checkQuote(sub, q);
}

void MDSubscriber::onOrderReplace(const MDSubscription *sub, const MDOrder *o, const MDOrder * n)
{


	if (_app->inMetricsMode())
	{
		calculateMetrics(sub, n);
		return;
	}
	if (_logger->isDebugEnabled())
	{
		_logger->logV(MD_DEBUG_MSG,"NEW Order Replace [%s] replaces [%s] for instrument[%s]" , const_cast<MDOrder*> (n)->toString(),
		const_cast<MDOrder*> (o)->toString(), sub->instrument().toString());
	}
	if (o) printOrder(sub, o);
	if (o) checkOrder(sub, o);
	if (n) printOrder(sub, n);
	if (n) checkOrder(sub, n);
	if (o) printProduct(sub, o, sub->product());
}

void MDSubscriber::onOrder(const MDSubscription *sub, const MDOrder *o, const MDTrade * t)
{
	// time verification
	//if (o->_recordedTime.tv_sec > 0)
	//{
//		int secdiff = o->_recordedTime.tv_sec - o->_exchangeTime.tv_sec;
//		cout << "Time Difference is " << (secdiff / 3600) << " hours " << endl;
//	}
	if (_app->inMetricsMode())
	{
		calculateMetrics(sub, o);
		return;
	}
	if (_logger->isDebugEnabled())
	{
		_logger->logV(MD_DEBUG_MSG,"NEW Order[%s] for instrument[%s]" , const_cast<MDOrder*> (o)->toString(), sub->instrument().toString());
	}
	if (_logger->isDebugEnabled() && t)
	{
		_logger->logV(MD_DEBUG_MSG,"NEW Trade (with order) [%s] for instrument[%s]", const_cast<MDTrade *> (t)->toString(), sub->instrument().toString());
	}
	if (o) printOrder(sub, o);
	if (o) checkOrder(sub, o);
	if (t) printTrade(sub, t);
	if (t) checkTrade(sub, t);
	if (o) printProduct(sub, o, sub->product());
}

void MDSubscriber::onImbalance(const MDSubscription *sub, const MDImbalance *i)
{
	if (_logger->isDebugEnabled())
	{
		_logger->logV(MD_DEBUG_MSG,"NEW Imbalance[%s] for instrument[%s]" , const_cast<MDImbalance*> (i)->toString(), sub->instrument().toString());
	}
	if (i) printImbalance(sub, i);
}

void MDSubscriber::onTrade(const MDSubscription* sub, const MDTrade* t)
{
	if (_app->inMetricsMode())
	{
		calculateMetrics(sub, t);
		return;
	}
	if (_logger->isDebugEnabled())
	{
		_logger->logV(MD_DEBUG_MSG,"NEW Trade[%s] for instrument[%s]" , const_cast<MDTrade *> (t)->toString(), sub->instrument().toString());
	}
	printTrade(sub, t);
	checkTrade(sub, t);
	printProduct(sub, t, sub->product());
}

void MDSubscriber::onSecurityStatusChange(const InstrumentDef* instrument)
{
	if (_logger->isDebugEnabled())
	{
		_logger->logV(MD_DEBUG_MSG,"Security Status Change for instrument[%s]" , instrument->toString());
	}
	if (_dqcheck)
	{
		assert(instrument->subscription() != 0);
		assert(instrument->subscription()->instrument().state() == instrument->state());
		assert(0 == strcmp(instrument->subscription()->instrument().key(), instrument->key()));
	}
	if (instrument->subscription() != 0)
	{
		printSecurityStatus(instrument->subscription(), instrument);
	}

}
void MDSubscriber::onSubscriptionInvalid(const MDSubscription *sub, const char *reason)
{
	if (sub->book())
	{
		_logger->logV(MD_WARN_MSG,"Subscription invalid [%s] reason=%s, book now:=%s" , sub->instrument().toString(), reason, sub->book()->toString());
	}
	else
	{
		_logger->logV(MD_WARN_MSG, "Subscription invalid [%s] reason=%s", sub->instrument().toString(), reason);
	}

}

MDSubscriber::MDSubscriber() :
	_shutdown(false),_mainshutdown(false),_csvfile(0) 
#ifndef __GNUATOMICOPS__
			, _mmutex(false)
#endif
	, _writefilemutex(false)
{
	_instruments = new std::vector<std::pair<InstrumentDef,std::string> >;
	_latencyResults = 0;
	_mdrcLatencyResults = 0;
	_mdSMList.clear();
	_maxEvents = 0;
	_eventCount = 0;
	_pctCount = 100;
	_latCount = 5;
	_printDepth = -1;
	_logct = 0;
	_autoload = false;
}

extern char* optarg;
bool MDSubscriber::parseCmdLine(int ac, char** av)
{
	md_test::MDSubscriber::_gHandle = this;
	int c;
	std::string dqarg, configLocation, configFile, typeList, nameList, csvFile, maxEvents, symbols, latcount, pdepth, symbolNames;
	while ((c = getopt(ac, av, "am:l:c:t:n:f:x:s:S:d:e:p:?")) != -1)
	{
		switch (c)
		{
		case 'a':
			_autoload = true;
			break;
		case 'p':
			pdepth = optarg;
			break;
		case 'e':
			latcount = optarg;
			break;
		case 'd':
			dqarg = optarg;
			break;
		case 's':
			symbols = optarg;
			break;
		case 'S':
			symbolNames = optarg;
			break;
		case 'x':
			maxEvents = optarg;
			break;
		case 'f':
			csvFile = optarg;
			break;
		case 't':
			typeList = optarg;
			break;
		case 'n':
			nameList = optarg;
			break;
		case 'l':
			configLocation = optarg;
			break;
		case 'c':
			configFile = optarg;
			break;
		case '?':
		default:
			return false;
		}
	}

	try
	{
		_app = MDApplication::instance();
	}
	catch (const ConfigException& ce)
	{
		cerr << ce.what() << endl;
	}
	catch (const ResourceException& re)
	{
		cerr << re.what() << endl;
	}
	catch (...)
	{
		cerr << "Unknown exception in app. " << endl;
	}

	if (!_app)
	{
		cerr << "Unable to get instance of MDApplication" << endl;
		return false;
	}

	if (typeList.empty() && !_autoload)
	{
		cerr << "No runtime MD SessionMgrType provided. " << endl;
		return false;
	}
	if (nameList.empty() && !_autoload)
	{
		cerr << "No runtime MD SessionMgr Name provided. " << endl;
		return false;
	}
	if (!symbols.empty())
	{
		_symbolfile = symbols;
	}
	if (!symbolNames.empty())
	{
		_symbolnames = symbolNames;
	}
#ifdef __ASSERT_ENABLED__
	_dqcheck = true;
#else
	_dqcheck = false;
#endif
	if (!dqarg.empty())
	{
		if (0 == dqarg.compare("true"))
		{
			_dqcheck = true;
		}
		else
		{
			_dqcheck = false;
		}
		if (_dqcheck)
		{
			cerr << "Data quality checks are ENABLED... debug assertions may occur!" << endl;
		}
	}

	if (!csvFile.empty())
	{
		if (0 == csvFile.compare("-") || 0 == csvFile.compare("/dev/stdout"))
		{
			_csvfile = stdout;
			cerr << "Writing to STDOUT" << endl;
		}
		else
		{
			//make fprintf's a little efficient. Its freaking silly to do fprintf's for oodles of data without a buffer
			cerr << "Writing to csv output file " << csvFile << endl;
#ifdef _WINDOWS
//			int flags = _O_CREAT | _O_TRUNC | _O_RDWR | _O_BINARY;//NOTE: Bitwise OR of LARGE FILE.
//			int mode = _S_IREAD | _S_IWRITE;
//			int fd = (_open)(csvFile.c_str(), flags, mode);
			_csvfile = fopen(csvFile.c_str(), "w");
#else
			int flags = O_CREAT | O_TRUNC | O_RDWR | O_LARGEFILE;//NOTE: Bitwise OR of LARGE FILE.
			int mode = S_IRGRP | S_IROTH | S_IRUSR | S_IWUSR;
			int fd = (::open)(csvFile.c_str(), flags, mode);
			_csvfile = (::fdopen)(fd, "w+");
#endif
			if (_csvfile)
			{
				char* fbuf = (char*) malloc(16 * 1024);
#ifdef _WINDOWS
				setbuf(_csvfile, fbuf);
#else
				setbuffer(_csvfile, fbuf, 16 * 1024);
#endif
			}
		}

		if (_csvfile == NULL)
		{
			cerr << "Unable to open file for writing!" << endl;
			return false;
		}
		if (_csvfile != NULL && !pdepth.empty())
		{
			_printDepth = atoi(pdepth.c_str());
			cerr << "NOTE: Maximum printed depth of book printed to file is " << _printDepth << endl;
		}
		if (!maxEvents.empty())
		{
			_maxEvents = atoi(maxEvents.c_str());
			_pctCount = _maxEvents / 20;
			cerr << "NOTE: Program will terminate after printing " << _maxEvents << " lines to file " << csvFile << endl;
		}
	}
	if (_app->inMetricsMode())
	{
		if (!latcount.empty())
		{
			_latCount = atoi(latcount.c_str());
		}
		else
		{
			_latCount = 10;
		}
		cerr << "NOTE: Latency count is " << _latCount << endl;
		if (!maxEvents.empty())
		{
			bool needmdrc = false;
			needmdrc = (typeList.find("MDRCClientSessionMgr", 0) != string::npos); //hard coded so no includes
			_maxEvents = atoi(maxEvents.c_str());
#ifdef _LINUX_
			// protection
			long long mult = needmdrc ? 2 : 1;
			long long mb = (1024 * 1024);
			struct sysinfo info;
			sysinfo(&info);
			//cout << "RAM: " << info.freeram << endl;
			long long maxbytes = info.freeram / 2;
			if (mult * _maxEvents * sizeof(LResult) > (maxbytes))
			{
				cerr << "ERROR: Max Events of " << _maxEvents << " would use up " << (mult * _maxEvents * sizeof(LResult)) / mb << " Mb of memory, max is " << (maxbytes / mb)
						<< " Mb. Please use a smaller value." << endl;
				return false;
			}
			cerr << "NOTE: Size of event array will be " << (mult * _maxEvents * sizeof(LResult)) / mb << " Mb which is " << (mult * _maxEvents * sizeof(LResult) * 100 / maxbytes)
					<< "% of maximum allowed (" << (maxbytes / mb) << "Mb)" << endl;

#endif
			_latencyResults = new LResult[_maxEvents];
			if (needmdrc)
			{
				cerr << "NOTE: Creating MDRC Latency Structure" << endl;
				_mdrcLatencyResults = new LResult[_maxEvents];
			}
			_pctCount = _maxEvents / 20;
			cerr << "NOTE: Program will terminate after counting " << _maxEvents << " latency events" << endl;
		}
	}
	if (!_autoload)
	{
		std::vector<std::string> types;
		std::vector<std::string> names;
		StringUtil::tokenize(types, typeList, ',');
		StringUtil::tokenize(names, nameList, ',');
		if (types.size() != names.size())
		{
			cerr << "Must provide the same number of MDSessionMgr types and names. " << endl;
			return false;
		}
		int sz = types.size();
		for (int i = 0; i < sz; i++)
		{
			_typeNameList.push_back(std::pair<std::string, std::string>(types[i], names[i]));
		}
	}
	else
	{
		cerr << "Will initalize all session managers found in the file" << endl;
	}
	//register signal handlers for shutdown
#ifndef _WINDOWS
	signal(SIGINT, MDSubscriber::sigH);
	signal(SIGTERM, MDSubscriber::sigH);
#endif
	try
	{
		_config = _app->load(configLocation, configFile, __MD_VERSION__);
		_logger = _app->getLogger();
		cerr << "**************************************************" << endl;
		cerr << "Use CTRL-C(if running in forground) or kill -2 | kill -15 to terminate subscriber" << endl;
		cerr << "**************************************************" << endl;
		return true;
	}
	catch (const ConfigException& ce)
	{
		cerr << ce.what() << endl;
	}
	catch (const ResourceException& re)
	{
		cerr << re.what() << endl;
	}
	catch (...)
	{
		cerr << "Unknown exception in main. " << endl;
	}
	return false;
}

void MDSubscriber::sigH(int sig)
{
	if (_sigct || _gHandle->_mdSMList.empty())
	{
		exit(sig);
	}
	_sigct++;
	_gHandle->shutdown();
}

int main(int ac, char** av)
{
	md_test::MDSubscriber* sub = new md_test::MDSubscriber();
	while (!sub->parseCmdLine(ac, av))
	{
		cerr << "usage: mdsub -l <config location> -c <config file> [-a] [-t <SessionMgrType> -n <SessionMgrName>]  [-f csvfile] [-x maxevents] [-s symbolfile] [-S symbols] [-d true] [-p depth] [-e latencyevents]" << endl;
		cerr << "-a autoload: initialize and subscribe to all session managers found in XML (use instead of -t and -n options)" << endl;
		cerr << "-t comma separated list of session manager types found in XML, must be used with -n (not required if autoload is specified)" << endl;
		cerr << "-n comma separated list of session manager names found in XML, must be used with -t (not required if autoload is specified)" << endl;
		cerr << "-f outputs all observed data to the file specified" << endl;
		cerr << "-x will automatically terminate mdsub after [maxevents] is written to the file" << endl;
		cerr << "-e in metrics mode log only every this number of events, default 5" << endl;
		cerr << "-p file output will only print maximum of this depth, default unlimited" << endl;
		cerr << "-S reads symbol from command line (comma separated, periods are name/suffix split)" << endl;
		cerr << "-s loads symbols from a file instead of from configuration, one symbol per line" << endl;
		cerr << "   Note that -S overrides -s which overrides the xml symbol list" << endl;
		cerr << "-d enables data quality checks, [true] for standard checks" << endl;	
		return -1;
	}

	try
	{
		sub->init();
		sub->start();
	}
	catch (const ConfigException& ce)
	{
		cerr << ce.what() << endl;
	}
	catch (const ResourceException& re)
	{
		cerr << re.what() << endl;
	}
	return 0;
}

void MDSubscriber::checkQuote(const md_core::MDSubscription* sub, const md_core::MDQuote* quote)
{
	if (!_dqcheck) return;

	bool allownegative = sub->getSM()->allowNegativeBookPrices();

	// quotes can be one sided
	if (!allownegative)
	{
		assert(quote->_ask >= 0);
		assert(quote->_bid >= 0);
		assert(quote->_askSize >= 0);
		assert(quote->_bidSize >= 0);
	}
	if (quote->_isNBBO)
	{
		assert(quote->bidexchange()[0] != 0);
		assert(strlen(quote->bidexchange()) <= sizeof(quote->_bexchange));
		assert(quote->offerexchange()[0] != 0);
		assert(strlen(quote->bidexchange()) <= sizeof(quote->_oexchange));
	}
	assert(quote->_seqno > 0);
	assert(quote->_originalseqno > 0);
}

void MDSubscriber::checkTrade(const md_core::MDSubscription* sub, const md_core::MDTrade* trade)
{
	if (!_dqcheck) return;

	bool allownegative = sub->getSM()->allowNegativeBookPrices();

	if (!allownegative)
	{
		assert(trade->_last> 0);
	}
	if (trade->_status != MDTrade::BUST)
	{
		assert(trade->_lastSize> 0);
	}
	assert(trade->_seqno> 0);
	assert(trade->_originalseqno> 0);
}
void MDSubscriber::checkBook(const md_core::MDSubscription* sub, const md_core::MDOrderBook* book)
{
	if (!_dqcheck) return;

	bool allownegative = book->allowNegativeBookPrices();
	double prev = LONG_MAX;

	MDPriceLevel * plb = book->getBuys();
	for (int i = 0; i < plb->getCurrentSize(); i++)
	{
		const MDPriceLevelInternal * pi = plb->operator[](i);
		if (!allownegative)
		{
			assert(pi->_px > 0);
		}
		if (book->isComposite())
		{
			assert(pi->_px <= prev);
		}
		else
		{
			assert(pi->_px < prev);
		}
		prev = pi->_px;
		assert(pi->_size > 0);
		if (pi->_numorders!=-1) assert(pi->_numorders >= 0); // allow implied prices

		if (sub->getSM()->bookMaintainsOrders())
		{
			if (strcmp(pi->_venue, sub->book()->source())) continue;	// can't look at order list from other feeds
			assert(pi->_orders);
			assert(pi->_orders->size() == pi->_numorders);
			std::list<MDOrder*>::iterator oi = pi->_orders->begin();
			while (oi != pi->_orders->end())
			{
				MDOrder * order = *oi;
				assert(0 == MDUtil::compare(order->_price, pi->_px, sub->getSM()->priceThreshold()));
				assert(order->_size > 0);
				assert(order->_status != MDOrder::DELETE);
				assert(order->_side == OrderDefs::BUY);
				assert(order->_shared_ct > 0);
				++oi;
			}
		}
	}

	if (allownegative)
	{
		prev = 0 - (double) LONG_MAX;
	}
	else
	{
		prev = 0;
	}
	plb = book->getSells();
	for (int i = 0; i < plb->getCurrentSize(); i++)
	{
		const MDPriceLevelInternal * pi = plb->operator[](i);
		if (!allownegative)
		{
			assert(pi->_px > 0);
		}
		if (book->isComposite())
		{
			assert(pi->_px >= prev);
		}
		else
		{
			assert(pi->_px > prev);
		}
		prev = pi->_px;
		assert(pi->_size > 0);
		if (pi->_numorders!=-1) assert(pi->_numorders >= 0); // allow implied prices

		if (sub->getSM()->bookMaintainsOrders())
		{
			if (strcmp(pi->_venue, sub->book()->source())) continue; // can't change order
			assert(pi->_orders);
			assert(pi->_orders->size() == pi->_numorders);
			std::list<MDOrder*>::iterator oi = pi->_orders->begin();
			while (oi != pi->_orders->end())
			{
				MDOrder * order = *oi;
				assert(0 == MDUtil::compare(order->_price, pi->_px, sub->getSM()->priceThreshold()));
				assert(order->_size > 0);
				assert(order->_status != MDOrder::DELETE);
				assert(order->_side == OrderDefs::SELL);
				assert(order->_shared_ct > 0);
				++oi;
			}
		}
	}
}
void MDSubscriber::checkOrder(const md_core::MDSubscription* sub, const md_core::MDOrder* order)
{
	if (!_dqcheck) return;
	bool allownegative = sub->getSM()->allowNegativeBookPrices();
	assert(order->_side != OrderDefs::UNKNOWN);
	if (order->_status != MDOrder::DELETE)
	{
		assert(order->_size> 0);
		if (order->_status == MDOrder::MODIFY)
		{
			assert(order->_oldsize > 0);
			assert(order->_oldprice != 0.0);
		}
	}
	else
	{
		assert(order->_size == 0);
		assert(order->_oldsize > 0);
		assert(order->_oldprice != 0.0);
	}
	if (!allownegative)
	{
		assert(order->_price> 0);
	}
	assert(order->_oid.isValid());
	assert(order->_seqno> 0);
	assert(order->_originalseqno> 0);
}

void MDSubscriber::printImbalance(const MDSubscription* sub, const MDImbalance* im)
{
	bool ss = false;
	{
		SimpleLock lk(_writefilemutex);
		//if (_csvfile == NULL) return;
		ss = printHeader('I', sub, im, 0);
		if (!ss)
		{
			char _buffer[8000];
			char itype;
			switch (im->_when)
			{
			case MDImbalance::IMB_NONE:
				itype = 'N';
				break;
			case MDImbalance::OPEN_IMB:
				itype = 'O';
				break;
			case MDImbalance::CLOSE_IMB:
				itype = 'C';
				break;
			case MDImbalance::NO_OPEN_IMB:
				itype = 'P';
				break;
			case MDImbalance::NO_CLOSE_IMB:
				itype = 'L';
				break;
			case MDImbalance::IMB_OTHER:
				itype = 'T';
				break;
			default:
				itype = 'U';
				break;
			}
			string ct(im->_crosstype, sizeof(im->_crosstype));
			if (_csvfile == NULL)
			{
				sprintf(_buffer, ",%c,%c,%s,%f,%f,%f,%f,%f,%ld,%ld,%ld,%ld\n", itype, printChar(im->_regulatory,'-'), ct.c_str(), im->_price, im->_farprice, im->_nearprice, im->_bp, im->_ap, im->_size, im->_pairedsize, im->_totalImbalance,
						im->_marketImbalance);
			}
			else
			{
				fprintf(_csvfile, ",%c,%c,%s,%f,%f,%f,%f,%f,%ld,%ld,%ld,%ld\n", itype, printChar(im->_regulatory,'-'), ct.c_str(), im->_price, im->_farprice, im->_nearprice, im->_bp, im->_ap, im->_size, im->_pairedsize, im->_totalImbalance,
						im->_marketImbalance);
			}
		}
	}
	if (ss) shutdown();
}

void MDSubscriber::printSecurityStatus(const md_core::MDSubscription* sub, const md_core::InstrumentDef* instrument)
{
	bool ss = false;
	{
		SimpleLock lk(_writefilemutex);
		//if (_csvfile == NULL) return;
		ss = printHeader('S', sub, 0, instrument);
	}
	if (ss) shutdown();
}

void MDSubscriber::printTrade(const MDSubscription* sub, const MDTrade* trade)
{
	bool ss = false;
	{
		SimpleLock lk(_writefilemutex);
		//if (_csvfile == NULL) return;
		ss = printHeader('T', sub, trade, 0);
		if (!ss)
		{
			char _buffer[8000];
			char visible;
			switch (trade->_visibility)
			{
				case MDTrade::TV_HIDDEN: visible = 'H'; break;
				case MDTrade::TV_VISIBLE: visible = 'V'; break;
				case MDTrade::TV_UNKNOWN: visible = 'U'; break;
				default: visible = '?'; break;
			}


			if (trade->_oid.isValid())
			{
				char oidbuff[OIDSIZE + 1];
				trade->orderID(oidbuff, sizeof(oidbuff));
				if (_csvfile == NULL)
				{
					sprintf(_buffer, ",%s", oidbuff);
				}
				else
				{
					fprintf(_csvfile, ",%s", oidbuff);
				}
			}
#if ( (_WINDOWS) && (_MSC_VER < 1400) )
			if (_csvfile == NULL)
			{
				sprintf(_buffer, ",%s,%f,%ld,%d,%lu,%s,%c\n", trade->_side < OrderDefs::UNKNOWN || trade->_side > OrderDefs::CROSS_SHORT ? "Invalid" : OrderDefs::_sidesS[(int) trade->_side], trade->_last,
						trade->_lastSize, trade->_status, (unsigned long)trade->_cumVolume, trade->_compositeParticipantID, visible);
			}
			else
			{
				fprintf(_csvfile, ",%s,%f,%ld,%d,%lu,%s,%c\n", trade->_side < OrderDefs::UNKNOWN || trade->_side > OrderDefs::CROSS_SHORT ? "Invalid" : OrderDefs::_sidesS[(int) trade->_side], trade->_last,
						trade->_lastSize, trade->_status, (unsigned long)trade->_cumVolume, trade->_compositeParticipantID, visible);
			}
#else
			if (_csvfile == NULL)
			{
				sprintf(_buffer, ",%s,%f,%ld,%d,%llu,%s,%c\n", trade->_side < OrderDefs::UNKNOWN || trade->_side > OrderDefs::CROSS_SHORT ? "Invalid" : OrderDefs::_sidesS[(int) trade->_side], trade->_last,
						trade->_lastSize, trade->_status, trade->_cumVolume, trade->_compositeParticipantID, visible);
			}
			else
			{
				fprintf(_csvfile, ",%s,%f,%ld,%d,%llu,%s,%c\n", trade->_side < OrderDefs::UNKNOWN || trade->_side > OrderDefs::CROSS_SHORT ? "Invalid" : OrderDefs::_sidesS[(int) trade->_side], trade->_last,
						trade->_lastSize, trade->_status, trade->_cumVolume, trade->_compositeParticipantID, visible);
			}
#endif

		}
	}
	if (ss) shutdown();
}

void MDSubscriber::printOrder(const MDSubscription* sub, const MDOrder* order)
{
	bool ss = false;
	{
		SimpleLock lk(_writefilemutex);
		//if (_csvfile == NULL) return;
		ss = printHeader('O', sub, order, 0);
		if (!ss)
		{
			char _buffer[8000];
			char oidbuff[OIDSIZE + 1];
			order->orderID(oidbuff, sizeof(oidbuff));
			if (_csvfile == NULL)
			{
				sprintf(_buffer, ",%s", oidbuff);
			}
			else
			{
				fprintf(_csvfile, ",%s", oidbuff);
			}

			char status;
			switch (order->_status)
			{
			case MDOrder::ADD:
				status = 'A';
				break;
			case MDOrder::MODIFY:
				status = 'M';
				break;
			case MDOrder::DELETE:
				status = 'D';
				break;
			default:
				status = 'I';
				break;
			}
			if (_csvfile == NULL)
			{
				sprintf(_buffer, ",%c,%s,%f,%ld,%f,%ld", status, order->_side < OrderDefs::UNKNOWN || order->_side > OrderDefs::CROSS_SHORT ? "Invalid" : OrderDefs::_sidesS[(int) order->_side],
						order->_price, order->_size, order->_oldprice, order->_oldsize);
			}
			else
			{
				fprintf(_csvfile, ",%c,%s,%f,%ld,%f,%ld", status, order->_side < OrderDefs::UNKNOWN || order->_side > OrderDefs::CROSS_SHORT ? "Invalid" : OrderDefs::_sidesS[(int) order->_side],
						order->_price, order->_size, order->_oldprice, order->_oldsize);
			}
			if (order->_display)
			{
				if (_csvfile == NULL)
				{
					sprintf(_buffer, ",%c\n",order->_display);
				}
				else
				{
					fprintf(_csvfile, ",%c\n",order->_display);
				}
			}
			else
			{
				if (_csvfile == NULL)
				{
					sprintf(_buffer, "\n");
				}
				else
				{
					fprintf(_csvfile, "\n");
				}
			}
		}
	}
	if (ss) shutdown();
}

bool MDSubscriber::printHeader(char type, const MDSubscription* sub, const MDBaseData * base, const InstrumentDef* instrument)
{
	static int count = 0;
	if (_maxEvents > 0 && count == _maxEvents)
	{
		cerr << "Wrote " << count << " lines, terminating " << endl;
		++count;
		return true;
	}
	else if (_maxEvents > 0 && count > _maxEvents)
	{
		return true;
	}
	else if (_shutdown)
	{
		return true;
	}
	++count;
	if (base)
	{
		struct tm tout;
		time_t localtime = base->_exchangeTime.tv_sec;
		MDTime::_localtime(&(localtime), &tout);
		::strftime(_timebuffer, sizeof(_timebuffer), "%H:%M:%S", &tout);
#if ( (_WINDOWS) && (_MSC_VER < 1400) )
		if (_csvfile == NULL)
		{
			char _buffer[8000];
			sprintf(_buffer, "%s,%lu,%c,%s.%06ld", sub->instrument().key(), (unsigned long)base->_originalseqno, type, _timebuffer, base->_exchangeTime.tv_usec);
		}
		else
		{
			fprintf(_csvfile, "%s,%lu,%c,%s.%06ld", sub->instrument().key(), (unsigned long)base->_originalseqno, type, _timebuffer, base->_exchangeTime.tv_usec);
		}
#else
		if (_csvfile == NULL)
		{
			char _buffer[8000];
			sprintf(_buffer, "%s,%llu,%c,%s.%06ld", sub->instrument().key(), base->_originalseqno, type, _timebuffer, base->_exchangeTime.tv_usec);
		}
		else
		{
			fprintf(_csvfile, "%s,%llu,%c,%s.%06ld", sub->instrument().key(), base->_originalseqno, type, _timebuffer, base->_exchangeTime.tv_usec);
		}
#endif
	}
	else
	{
		if (_csvfile == NULL)
		{
			char _buffer[8000];
			sprintf(_buffer, "%s,-,%c,-,%s\n", instrument->key(), type, instrument->toString());
		}
		else
		{
			fprintf(_csvfile, "%s,-,%c,-,%s\n", instrument->key(), type, instrument->toString());
		}
	}
	return false;

}


void MDSubscriber::printTradeDissemination(const MDSubscription* sub, const MDMktTradeDisseminationTimeAlert* alert)
{
	bool ss = false;
	{
		SimpleLock lk(_writefilemutex);
		//if (_csvfile == NULL) return;
		ss = printHeader('D', sub, alert, 0);
		if (!ss)
		{
			if (_csvfile == NULL)
			{
				char _buffer[8000];
				sprintf(_buffer, ",%c,%c,%c,%s\n", printChar(alert->_financialStatus,'-'),  printChar(alert->_corporateAction,'-'), printChar(alert->_securityStatus,'-'), alert->_tradeDisseminationTime);
			}
			else
			{
				fprintf(_csvfile, ",%c,%c,%c,%s\n", printChar(alert->_financialStatus,'-'),  printChar(alert->_corporateAction,'-'), printChar(alert->_securityStatus,'-'), alert->_tradeDisseminationTime);
			}
		}
	}
	if (ss) shutdown();
}

void MDSubscriber::printTradeIndication(const MDSubscription* sub, const MDMktTradingIndicationAlert* alert)
{
	bool ss = false;
	{
		SimpleLock lk(_writefilemutex);
		//if (_csvfile == NULL) return;
		ss = printHeader('N', sub, alert, 0);
		if (!ss)
		{
			if (_csvfile == NULL)
			{
				char _buffer[8000];
				sprintf(_buffer, ",%c,%c,%c,%c,%d,%d,%d,%f,%f\n", printChar(alert->_financialStatus,'-'),  printChar(alert->_corporateAction,'-'), printChar(alert->_securityStatus,'-'),
				+ printChar(alert->_adjustment,'-'), alert->_pdenom, alert->_bid, alert->_ask,alert->_bidP,alert->_askP );
			}
			else
			{
				fprintf(_csvfile, ",%c,%c,%c,%c,%d,%d,%d,%f,%f\n", printChar(alert->_financialStatus,'-'),  printChar(alert->_corporateAction,'-'), printChar(alert->_securityStatus,'-'),
				+ printChar(alert->_adjustment,'-'), alert->_pdenom, alert->_bid, alert->_ask,alert->_bidP,alert->_askP );
			}
		}
	}
	if (ss) shutdown();
}


void MDSubscriber::printQuote(const MDSubscription* sub, const MDQuote* quote, char qtype)
{
	bool ss = false;
	{
		SimpleLock lk(_writefilemutex);
		//if (_csvfile == NULL) return;
		ss = printHeader('Q', sub, quote, 0);
		if (!ss)
		{
			if (_csvfile == NULL)
			{
				char _buffer[8000];
				sprintf(_buffer, ",%c,%s,%f,%ld,%f,%ld,%f,%f,%f,%d,%d,%s,%s,%s\n", qtype, MDQuote::toStringStatus(quote->_status), quote->_bid, quote->_bidSize, quote->_ask, quote->_askSize,
						0.0, 0.0, 0.0, quote->_numbuyorders, quote->_numsellorders, quote->isNBBO() ? "Y" : "N", quote->bidexchange(), quote->offerexchange());
			}
			else
			{
				fprintf(_csvfile, ",%c,%s,%f,%ld,%f,%ld,%f,%f,%f,%d,%d,%s,%s,%s\n", qtype, MDQuote::toStringStatus(quote->_status), quote->_bid, quote->_bidSize, quote->_ask, quote->_askSize,
						0.0, 0.0, 0.0, quote->_numbuyorders, quote->_numsellorders, quote->isNBBO() ? "Y" : "N", quote->bidexchange(), quote->offerexchange());
			}
		}
	}
	if (ss) shutdown();
}

void MDSubscriber::printBook(const MDSubscription* sub, const MDOrderBook* book)
{
	bool ss = false;
	{
		SimpleLock lk(_writefilemutex);
		//if (_csvfile == NULL) return;
		ss = printHeader('B', sub, book, 0);
		if (!ss)
		{
			char _buffer[8000];
			bool comp = book->isComposite();
			// print buys
			MDPriceLevel * plb = book->getBuys();

			int depth = plb->getCurrentSize();
			if (_printDepth > 0 && depth > _printDepth) depth = _printDepth;
			if (_csvfile == NULL)
			{
				sprintf(_buffer, ",B:%d[", depth);
			}
			else
			{
				fprintf(_csvfile, ",B:%d[", depth);
			}
			for (int i = 0; i < depth; i++)
			{
				const MDPriceLevelInternal * pi = plb->operator[](i);
				if (_csvfile == NULL)
				{
					if (i > 0) sprintf(_buffer, "|%f,%ld,%d,%d", pi->_px, pi->_size, pi->_numorders, pi->_changed);
					else sprintf(_buffer, "%f,%ld,%d,%d", pi->_px, pi->_size, pi->_numorders, pi->_changed);
					if (comp) sprintf(_buffer, ",%s", pi->_venue);
				}
				else
				{
					if (i > 0) fprintf(_csvfile, "|%f,%ld,%d,%d", pi->_px, pi->_size, pi->_numorders, pi->_changed);
					else fprintf(_csvfile, "%f,%ld,%d,%d", pi->_px, pi->_size, pi->_numorders, pi->_changed);
					if (comp) fprintf(_csvfile, ",%s", pi->_venue);
				}
			}

			plb = book->getSells();
			depth = plb->getCurrentSize();
			if (_printDepth > 0 && depth > _printDepth) depth = _printDepth;
			if (_csvfile == NULL)
			{
				sprintf(_buffer, "],S:%d[", depth);
			}
			else
			{
				fprintf(_csvfile, "],S:%d[", depth);
			}
			for (int i = 0; i < depth; i++)
			{
				const MDPriceLevelInternal * pi = plb->operator[](i);
				if (_csvfile == NULL)
				{
					if (i > 0) sprintf(_buffer, "|%f,%ld,%d,%d", pi->_px, pi->_size, pi->_numorders, pi->_changed);
					else sprintf(_buffer, "%f,%ld,%d,%d", pi->_px, pi->_size, pi->_numorders, pi->_changed);
					if (comp) sprintf(_buffer, ",%s", pi->_venue);
				}
				else
				{
					if (i > 0) fprintf(_csvfile, "|%f,%ld,%d,%d", pi->_px, pi->_size, pi->_numorders, pi->_changed);
					else fprintf(_csvfile, "%f,%ld,%d,%d", pi->_px, pi->_size, pi->_numorders, pi->_changed);
					if (comp) fprintf(_csvfile, ",%s", pi->_venue);
				}
			}

			if (_csvfile == NULL)
			{
				sprintf(_buffer, "]\n");
			}
			else
			{
				fprintf(_csvfile, "]\n");
			}

			if (sub->getSM()->bookMaintainsOrders())
			{
				MDPriceLevel * plb = book->getBuys();
				depth = plb->getCurrentSize();
				if (_printDepth > 0 && depth > _printDepth) depth = _printDepth;
				if (_csvfile == NULL)
				{
					sprintf(_buffer, "BUYS %d\n", depth);
				}
				else
				{
					fprintf(_csvfile, "BUYS %d\n", depth);
				}
				for (int i = 0; i < depth; i++)
				{
					const MDPriceLevelInternal * pi = plb->operator[](i);
					if (!pi->_orders)
					{
						if (_csvfile == NULL)
						{
							sprintf(_buffer, "NULL ORDER LIST\n");
						}
						else
						{
							fprintf(_csvfile, "NULL ORDER LIST\n");
						}
						continue;
					}

					if (_csvfile == NULL)
					{
						if (comp) sprintf(_buffer, "%s %d:\t", pi->_venue, (int)pi->_orders->size());
						else sprintf(_buffer, "%d:\t", (int)pi->_orders->size());
					}
					else
					{
						if (comp) fprintf(_csvfile, "%s %d:\t", pi->_venue, (int)pi->_orders->size());
						else fprintf(_csvfile, "%d:\t", (int)pi->_orders->size());
					}

					if (comp && strcmp(pi->_venue, sub->book()->source()))
					{
						if (_csvfile == NULL)
						{
							sprintf(_buffer, "\n");
						}
						else
						{
							fprintf(_csvfile, "\n");
						}
						continue;
					}

					std::list<MDOrder*>::iterator oi = pi->_orders->begin();
					while (oi != pi->_orders->end())
					{
						MDOrder * order = *oi;

						char oidbuff[OIDSIZE + 1];
						order->orderID(oidbuff, sizeof(oidbuff));
						if (_csvfile == NULL)
						{
							sprintf(_buffer, "%s", oidbuff);
						}
						else
						{
							fprintf(_csvfile, "%s", oidbuff);
						}

						char status;
						switch (order->_status)
						{
						case MDOrder::ADD:
							status = 'A';
							break;
						case MDOrder::MODIFY:
							status = 'M';
							break;
						case MDOrder::DELETE:
							status = 'D';
							break;
						default:
							status = 'I';
							break;
						}

						if (_csvfile == NULL)
						{
							sprintf(_buffer, ",%c,%s,%f,%ld,%f,%ld", status, order->_side < OrderDefs::UNKNOWN || order->_side > OrderDefs::CROSS_SHORT ? "Invalid" : OrderDefs::_sidesS[(int) order->_side],
									order->_price, order->_size, order->_oldprice, order->_oldsize);
						}
						else
						{
							fprintf(_csvfile, ",%c,%s,%f,%ld,%f,%ld", status, order->_side < OrderDefs::UNKNOWN || order->_side > OrderDefs::CROSS_SHORT ? "Invalid" : OrderDefs::_sidesS[(int) order->_side],
									order->_price, order->_size, order->_oldprice, order->_oldsize);
						}
						if (order->_display)
						{
							if (_csvfile == NULL)
							{
								sprintf(_buffer, ",%c #",order->_display);
							}
							else
							{
								fprintf(_csvfile, ",%c #",order->_display);
							}
						}
						else
						{
							if (_csvfile == NULL)
							{
								sprintf(_buffer, " # ");
							}
							else
							{
								fprintf(_csvfile, " # ");
							}
						}

						++oi;
					}

					if (_csvfile == NULL)
					{
						sprintf(_buffer, "\n");
					}
					else
					{
						fprintf(_csvfile, "\n");
					}
				}

				plb = book->getSells();
				depth = plb->getCurrentSize();
				if (_printDepth > 0 && depth > _printDepth) depth = _printDepth;
				if (_csvfile == NULL)
				{
					sprintf(_buffer, "SELLS %d\n", depth);
				}
				else
				{
					fprintf(_csvfile, "SELLS %d\n", depth);
				}
				for (int i = 0; i < depth; i++)
				{
					const MDPriceLevelInternal * pi = plb->operator[](i);
					if (!pi->_orders)
					{
						if (_csvfile == NULL)
						{
							sprintf(_buffer, "NULL ORDER LIST\n");
						}
						else
						{
							fprintf(_csvfile, "NULL ORDER LIST\n");
						}
						continue;
					}

					if (_csvfile == NULL)
					{
						if (comp) sprintf(_buffer, "%s %d:\t", pi->_venue, (int)pi->_orders->size());
						else sprintf(_buffer, "%d:\t", (int)pi->_orders->size());
					}
					else
					{
						if (comp) fprintf(_csvfile, "%s %d:\t", pi->_venue, (int)pi->_orders->size());
						else fprintf(_csvfile, "%d:\t", (int)pi->_orders->size());
					}

					if (comp && strcmp(pi->_venue, sub->book()->source()))
					{
						if (_csvfile == NULL)
						{
							sprintf(_buffer, "\n");
						}
						else
						{
							fprintf(_csvfile, "\n");
						}
						continue;
					}

					std::list<MDOrder*>::iterator oi = pi->_orders->begin();
					while (oi != pi->_orders->end())
					{
						MDOrder * order = *oi;

						char oidbuff[OIDSIZE + 1];
						order->orderID(oidbuff, sizeof(oidbuff));
						if (_csvfile == NULL)
						{
							sprintf(_buffer, "%s", oidbuff);
						}
						else
						{
							fprintf(_csvfile, "%s", oidbuff);
						}

						char status;
						switch (order->_status)
						{
						case MDOrder::ADD:
							status = 'A';
							break;
						case MDOrder::MODIFY:
							status = 'M';
							break;
						case MDOrder::DELETE:
							status = 'D';
							break;
						default:
							status = 'I';
							break;
						}

						if (_csvfile == NULL)
						{
							sprintf(_buffer, ",%c,%s,%f,%ld,%f,%ld", status, order->_side < OrderDefs::UNKNOWN || order->_side > OrderDefs::CROSS_SHORT ? "Invalid" : OrderDefs::_sidesS[(int) order->_side],
									order->_price, order->_size, order->_oldprice, order->_oldsize);
						}
						else
						{
							fprintf(_csvfile, ",%c,%s,%f,%ld,%f,%ld", status, order->_side < OrderDefs::UNKNOWN || order->_side > OrderDefs::CROSS_SHORT ? "Invalid" : OrderDefs::_sidesS[(int) order->_side],
									order->_price, order->_size, order->_oldprice, order->_oldsize);
						}
						if (order->_display)
						{
							if (_csvfile == NULL)
							{
								sprintf(_buffer, ",%c #",order->_display);
							}
							else
							{
								fprintf(_csvfile, ",%c #",order->_display);
							}
						}
						else
						{
							if (_csvfile == NULL)
							{
								sprintf(_buffer, " # ");
							}
							else
							{
								fprintf(_csvfile, " # ");
							}
						}

						++oi;
					}
					if (_csvfile == NULL)
					{
						sprintf(_buffer, "\n");
					}
					else
					{
						fprintf(_csvfile, "\n");
					}
				}
			}
		}
	}
	if (ss) shutdown();
}

void MDSubscriber::printProduct(const md_core::MDSubscription* sub, const md_core::MDBaseData * base, const md_core::MDProductInfo* product)
{
	bool ss = false;
	{
		SimpleLock lk(_writefilemutex);
		//if (_csvfile == NULL) return;
		if (product && product->complete() && product->_changed)
		{
			product->_changed = false;
		}
		else
		{
			return;
		}
		ss = printHeader('P', sub, base, 0);
		if (!ss)
		{
			if (_csvfile == NULL)
			{
				char _buffer[8000];
				sprintf(_buffer, ",%s\n", (const_cast<MDProductInfo*>(product))->toString().c_str());
			}
			else
			{
				fprintf(_csvfile, ",%s\n", (const_cast<MDProductInfo*>(product))->toString().c_str());
			}
		}
	}
	if (ss) shutdown();
}

void MDSubscriber::printCustom(const md_core::MDSubscription* sub, const md_core::MDCustomEvent* event)
{
	bool ss = false;
	{
		SimpleLock lk(_writefilemutex);
		//if (_csvfile == NULL) return;
		ss = printHeader('C', sub, event, 0);
		if (!ss)
		{
			if (_csvfile == NULL)
			{
				MDCustomEvent::CustomData price = (const_cast<MDCustomEvent*>(event))->getField(MDCustomEvent::EventFields::CF_OPEN_PRICE);
				char _buffer[8000];
				sprintf(_buffer, ",%s\n", (const_cast<MDCustomEvent*>(event))->toString());
			}
			else
			{
				fprintf(_csvfile, ",%s\n", (const_cast<MDCustomEvent*>(event))->toString());
			}
		}
	}
	if (ss) shutdown();
}
