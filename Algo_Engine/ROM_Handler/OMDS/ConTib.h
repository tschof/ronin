// ConTib.h: interface for the CRvCallback class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CONTIB_H__1B5545E8_FFB8_4C57_993B_D25B629D59EA__INCLUDED_)
#define AFX_CONTIB_H__1B5545E8_FFB8_4C57_993B_D25B629D59EA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "OMDSCommon.h"
#include "DTibMessage.h"

#include <rvcpp.h>
#include <rvevm.h>
#include "tibmsg.h"
#include <list>
#include "Lock.h"

#include "StockQuoteHandler.h"
#include "Stock2QuoteHandler.h"
#include "OptionQuoteHandler.h"
#include "OptionQuoteBestHandler.h"
#include "GreekHandler.h"
#include "FutureHandler.h"

#include "WinRvThread.h"
using namespace WinRVTool;

#define NUM_OF_MSG_PER_SECOND (1000)

class CDTibMessage;

typedef std::map<std::string, RvListener *>	StringRvListenerMap;

class CConnections;
class CDATib;
// =============================================================================
//	CConTib
// =============================================================================
class CConTib : public CRunable, public IDataCallback
{

public:
	CConTib( CConnections * pConnections );
	virtual ~CConTib();

	void				shutDown();
	void				run();
	bool				start();
	CDTibMessage *		getNextCDTibMessage();
	bool				isRunning(){ return true; }

	void				SubscribeStock( const std::string & szSymbol );
	void				SubscribeStock2( const std::string & szSymbol );
	void				SubscribeOption( const std::string & szBase, const std::string & szClass, const std::string & szExch );
	void				SubscribeOptionBest( const std::string & szBase, const std::string & szClass );
	void				SubscribeGreeks( const std::string & szBase );
	void				SubscribeFuture( const std::string & szBase, const std::string & szSymbol );

	void				UnSubscribeStock( const std::string & szSymbol );
	void				UnSubscribeStock2( const std::string & szSymbol );
	void				UnSubscribeOption( const std::string & szBase, const std::string & szClass, const std::string & szExch );
	void				UnSubscribeOptionBest( const std::string & szBase, const std::string & szClass );
	void				UnSubscribeGreeks( const std::string & szBase );
	void				UnSubscribeFuture( const std::string & szBase, const std::string & szSymbol );

	void				OnData( const std::string & szBase );

	void				addMessage( CDTibMessage * pTM );
	CDATib *			getDATib();
	COptionSeries		getOptionSeries( const std::string & szBase, const std::string & szEx = "", int nWait = 500 );
	CFutureSeries		getFutureSeries( const std::string & szBase );

private:
	void				newMessage();

	int					m_newSubjectCount;
	DataTibMsgList		m_tibMessageList;
	CConnections *		m_pConnections;
	Lock 				m_TibMessageListLock;

	WinRVThread			m_rvThread;
	bool				m_bShutdown;
public:
	CLog				m_log;

private: 
	StockQuoteHandler		* m_pStockHandler;
	Stock2QuoteHandler		* m_pStock2Handler;
	OptionQuoteHandler		* m_pOptionHandler;
	OptionQuoteBestHandler	* m_pOptionBestHandler;
	GreekHandler			* m_pGreekHandler;
	FutureHandler			* m_pFutureHandler;
};

#endif // !defined(AFX_CONTIB_H__1B5545E8_FFB8_4C57_993B_D25B629D59EA__INCLUDED_)

